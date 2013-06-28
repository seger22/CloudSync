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
#include<sys/stat.h>
#include <iomanip>
#include <string>
#include <sstream>
#include<fstream>
#include <vector>
#include "filehandler.h"
#include "ChunkHandler.h"
#include "chunkdat.h"
#include "boost/thread.hpp"

using boost::asio::ip::tcp;
using namespace std;


class session
{
public:
    session();
    session(boost::asio::io_service& io_service);
    tcp::socket& socket();
    void start();
    void read_request(const boost::system::error_code& error);
    void handle_write_request(const boost::system::error_code &err);
    void read_chunk_hashes(string filepath);
    void send_unmatched_hashes(const boost::system::error_code &err);
    void read_chunk_data(const boost::system::error_code &err, string filepath);
    void receive_file(string path,string name);
    void read_chunk_data_header(const boost::system::error_code &err);
    void temp_read_chunk_data();
    vector<int> get_unmatched_hashes(string data);
    void send_indexes(vector<int> indexes);


private:
    struct S{
        char chunk[65536];
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

    vector<ChunkDat> dt;
    vector<string> chunk_data;
};


#endif // SESSION_H
