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

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <iterator>

#include "chunkdat.h"
#include "Chunker.h"
#include "DBmanager.h"

using namespace std;

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

vector<Chunk*> processChunks(DataSource* ds,const char *path);
vector<Chunk*> chunk_file(const char *path);

void
chunk_directory(const char *dpath);
void createFile(vector<u_int64_t> t, vector<ChunkDat> chunks, string filepath);
vector<int> findMatch(vector<u_int64_t> hashesFromClient);
void DirectoryChecksum(const char *dpath);
bool findFileChecksum(string checksum, string &value);
void copyFile(string sourcepath,string destpath);

#endif //CHUNKHANDLER_H
