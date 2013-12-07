#ifndef CHUNKHANDLER_H
#define CHUNKHANDLER_H

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "rabinpoly.h"
#include <dirent.h>
#include <sys/stat.h>
#include<bitset>
#include <queue>
#include "blockchecksumserial.h"
#include "deltahandler.h"
#include "configurationmanager.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <iterator>

#include "chunkdat.h"
#include "Chunker.h"
#include "databasehandler.h"
#include "blockdat.h"

#include <assert.h>
#include "hash_chain.h"
#include "checksum.h"
#include "rollsum.h"
#include <queue>

using namespace std;
using namespace ulib;

#define INT64(n) n##LL
#define MSB64 INT64(0x8000000000000000)
#define FINGERPRINT_PT 0xbfe6b8a5bf378d83LL

typedef unsigned short BOOL;

const int FALSE = 0;
const int TRUE  = 1;

//utility functions probably belong somewhere else
//TODO handle varargs properly
#define DEBUG 0
//#define DEBUG 1


void debug(const char* msg, ...);
BOOL fileExists(const char* fname);
//end utility functions

struct BlockRowCol{
    int row;
    int col;
};

u_int64_t makeBitMask(int maskSize);
class Checksum
{
private:
    string checksum;
    string path;
public:
    Checksum(string c,string p){
        checksum = c;
        path=p;
    }

string getChecksum(){
        return checksum;
    }
string getPath(){
        return path;
    }
};

class ChunkBlockOrderer
{
private:
    int offset;
    bool Ischunk;

public:
    ChunkBlockOrderer(int offset, bool ischunk){
        this->offset=offset;
        this->Ischunk=ischunk;
    }

    int getOffset(){
        return offset;
    }

    int isChunk(){
        return Ischunk;
    }
};


class ChunkBoundaryChecker
{
  public:
  virtual BOOL isBoundary(u_int64_t fingerprint, int size) = 0;
};


class BitwiseChunkBoundaryChecker : public ChunkBoundaryChecker
{
  private:
    const int BITS=14;
    u_int64_t CHUNK_BOUNDARY_MASK;
    const int MAX_SIZE;
    const int MIN_SIZE;

  public:
  BitwiseChunkBoundaryChecker()
    : CHUNK_BOUNDARY_MASK(makeBitMask(BITS)),
      MAX_SIZE(4 * (1 << BITS)),
      MIN_SIZE(1 << (BITS - 2))
  {
    //printf("bitmask: %16llx\n", chunkBoundaryBitMask);
    //exit(0);
  }

  virtual BOOL isBoundary(u_int64_t fingerprint, int size)
  {
      return (((fingerprint & CHUNK_BOUNDARY_MASK) == 0 && size >= MIN_SIZE) ||
              (MAX_SIZE != -1 && size >= MAX_SIZE) );
  }

  int getMaxChunkSize() { return MAX_SIZE; }

};

class ChunkProcessor
{
private:
 unsigned int size;

//protected:
public:
  virtual void internalProcessByte(unsigned char c)
  {
     ++size;
//printf("processed byte %d\n", getSize());
  }

  virtual void internalCompleteChunk(u_int64_t hash, u_int64_t fingerprint)
  {
    size = 0;
  }

public:
  ChunkProcessor() : size(0) {}

  void processByte(unsigned char c)
  {
     internalProcessByte(c);
  }

  void completeChunk(u_int64_t hash, u_int64_t fingerprint)
  {
    internalCompleteChunk(hash, fingerprint);
  }

  virtual unsigned int getSize()
  {
    return size;
  }
};


class PrintChunkProcessor : public ChunkProcessor
{
private:
protected:
  virtual void internalCompleteChunk(u_int64_t hash, u_int64_t fingerprint)
  {
   // printChunkData("Found", getSize(), fingerprint, hash);
    ChunkProcessor::internalCompleteChunk(hash, fingerprint);
  }

public:
  PrintChunkProcessor()
  {
  }
};


class DataSource
{
public:
  virtual int getByte() = 0;
};


class RawFileDataSource : public DataSource
{
private:
  FILE* is;
public:
  RawFileDataSource(FILE* is) : is(is) {}
  ~RawFileDataSource()
  {
    fclose(is);
  }

  virtual int getByte()
  {
    return fgetc(is);
  }
};

void printChunkData(
    const char* msgPrefix,
    int size,
    u_int64_t fingerprint,
    u_int64_t hash);

void printChunkContents(
    u_int64_t hash,
    const unsigned char* buffer,
    int size);

//DeltaHandler* dlthandler;



vector<Chunk> processChunks(DataSource* ds,const char *path);
vector<Chunk> chunk_file(const char *path);
void deleteChunks(vector<Chunk*> chunks);

void chunk_directory(const char *dpath);
void insert_file_chunk(const char *path);
void createFile(vector<u_int64_t> t, vector<ChunkDat> chunks,string filepath);
vector<bool> findChunkMatch(vector<u_int64_t> hashesFromClient, vector<Chunk> *chunks, bool *allmatched, int &matchcount);
void DirectoryChecksum(const char *dpath);
bool findFileChecksum(string checksum, string value);
vector<bool> findBlockMatch(vector<BlockChecksumSerial> block_checksums, string filepath, vector<int> *offsets);
void createFileWithBlocks(vector<bool> matchdata, vector<int> offsets, vector<ChunkDat> blocks, string filepath, string filename);
void writeBlocks(vector<ChunkDat> blocks, vector<int> matchedOffsets, vector<int> allNewOffsets, string filepath, string filename);
void writeData(vector<Chunk> chunks, vector<ChunkDat> blocks, vector<int> matchedOffsets, vector<ChunkBlockOrderer> chunk_block_offsets,vector<int> lengths, string filename, string filepath);
void updateFileEntries(string path);
int getLastFileID(string path);
string fileChecksum(const char *path);

void writeChunks(vector<u_int64_t> hashesFromClient,queue<ChunkDat> *chunks, string filepath, string filename);
vector<BlockDat> getUncompressedBlocks(vector<ChunkDat> mergedBlocks, vector<int> lengths, vector<int> matchedOffsets);
void writeChunkFile(ChunkDat chunk, string filepath);

#endif //CHUNKHANDLER_H

