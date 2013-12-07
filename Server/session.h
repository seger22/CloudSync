#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include<boost/serialization/vector.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include<sys/stat.h>
#include <iomanip>
#include <queue>
#include <string>
#include <sstream>
#include<iterator>
#include<fstream>
#include <vector>
#include <exception>
#include "filehandler.h"
#include "chunkhandler.h"
#include "blockChecksumSerial.h"
#include "chunkdat.h"
#include <boost/thread.hpp>
#include <configurationmanager.h>
#include "chunkhash.h"

using boost::asio::ip::tcp;
using namespace std;


class session
{
public:
    session();
    session(boost::asio::io_service& io_service);
    tcp::socket& socket();
    void start();
    void read_request(const boost::system::error_code& error);//
    void read_chunk_data(string filepath, string filename, int unmatchcount);
    void receive_file(string path,string name);

    template<class T>
    vector<T> read_vector();
    template<class T>
    void send_vector(vector<T> v);    
    void file_modified(string filepath, string filename);
    void file_created(string filepath, string filename);    
    void read_block_data(vector<Chunk> chunks, vector<ChunkBlockOrderer> chunk_blocks,vector<int> matched_block_offsets,vector<int> lengths, string filepath, string filename);
    vector<ChunkBlockOrderer> mergeOffsets(vector<int> chunkOffsets, vector<int> blockOffsets, vector<int> *lengths);
    void read_from_client(char *p);
    bool read_request_from_client(char *p);

    unsigned int read_integer();
    unsigned char *read_buffer(int buf_size);
vector<BlockChecksumSerial> read_block_hashes();
vector<int> read_num_vector();
vector<u_int64_t> read_chunk_hashes();
void write_small_file(string filepath, string filename);

void send_integer(unsigned int n);

private:
    struct S{
        char chunk[66000];
    };

    string arg1,arg2,arg3;
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    string watch_dir;
    filehandler filehandler_;
    boost::asio::streambuf sbuf;
    vector<u_int64_t> filechunks;
    enum { max_chunk_length = 1024 };
    char chunk_[max_chunk_length];
    vector<u_int64_t> chunk_hashes;

 //   vector<ChunkDat> block;
    vector<string> chunk_data;
    queue<ChunkDat> chunkqueue;
};


#endif // SESSION_H
