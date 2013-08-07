
#include "session.h"

session::session(boost::asio::io_service &io_service):socket_(io_service)
{
    watch_dir="/home/mayuresan/Project/SyncDest";
}

tcp::socket& session::socket()
{
    return socket_;
}

void session::start()
{
    // cout<<"Session started"<<endl;
    socket_.async_read_some(boost::asio::buffer(data_, max_length), boost::bind(&session::read_request, this, boost::asio::placeholders::error));
}

void printnum(u_int64_t c){
    std::cout<<c<<endl;
}

void session::read_request(const boost::system::error_code &error){
    if (!error)
    {
        //  cout<<"read_request"<<endl;
        boost::system::error_code err;
        socket_.read_some(boost::asio::buffer(data_),err);
        if(err){
            cout<<"Client has stopped, so session stopping"<<endl;
            return;
        }

        // cout<<data_<<endl;

        std::vector<std::string> strs;
        boost::split(strs, data_, boost::is_any_of("|"));


        const char* req=strs.at(0).c_str();
        if(strcmp(req,"filecreated")==0){
            std::cout<<"File create command received"  <<std::endl;
            string checksum;
            bool matched = findFileChecksum(strs.at(3),checksum);

            char res[5];
            if(matched)
            {

                strcpy(res,"M");
                boost::asio::write(socket_,boost::asio::buffer(res));
            }
            else{
                strcpy(res,"U");
                boost::asio::write(socket_,boost::asio::buffer(res));

            }
            //this->receive_file(strs.at(1).c_str(), strs.at(2));
        }
        else if(strcmp(req,"filemodified")==0)
        {
            std::cout<<"File modified command received"  <<std::endl;
            //arg1=strs.at(1).c_str();
            //arg2=strs.at(2).c_str();
           // this->read_chunk_hashes(strs.at(1)+strs.at(2));
            this->file_modified(strs.at(1)+strs.at(2));

        }
        else if(strcmp(req,"dircreate")==0)
        {
            std::cout<<"Directory create command received"  <<std::endl;
            filehandler_.create_directory(strs.at(1).c_str(),strs.at(2).c_str());
        }

        else if(strcmp(req,"dirdelete")==0){
            std::cout<<"Directory delete command received"<<std::endl;
            filehandler_.delete_directory(strs.at(1).c_str(),strs.at(2).c_str());
        }
        else if(strcmp(req,"dirrename")==0){
            std::cout<<"Directory rename command received"<<std::endl;
            filehandler_.rename_directory(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
        }

        else if(strcmp(req,"filerename")==0){
            std::cout<<"File rename command received"<<std::endl;
            filehandler_.rename_file(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
        }

        else if(strcmp(req,"filemove")==0){
            std::cout<<"File move command received"<<std::endl;
            filehandler_.move_file(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
        }

        else if(strcmp(req,"dirmove")==0){
            std::cout<<"Directory move command received"<<std::endl;
            filehandler_.move_directory(strs.at(1).c_str(),strs.at(2).c_str());
        }

        else if(strcmp(req,"filedelete")==0){
            std::cout<<"File delete command received"<<std::endl;
            filehandler_.delete_file(strs.at(1).c_str(),strs.at(2).c_str());
        }

        strcpy(data_,"");
        this->read_request(err);

    }
    else
        cout<<"Oh No Something went wrong";
}


/*
 *void session::filemodified(){
 *read_chunk_hashes
 *match_chunks
 *send_match_result
 *read_unmatched_chunks_block_hash
 *read_block_match_result
 *read_block_data
 *}
 *
 *void session::filecreated(){
 *read_chunk_hashes
 *match_chunks
 *send_chunk_match_result
 *read_chunk_data
 *}
 *
 */

void session::file_modified(string filepath){
    chunk_hashes=read_vector<u_int64_t>();
    if(chunk_hashes.size()==0) return;
    vector<bool> unmatchedHashIndex = findMatch(chunk_hashes);
    cout<<"Sending unmatched hash indexes\n";
    send_vector<bool>(unmatchedHashIndex);
    cout<<"Sending block hashes\n";
    vector< vector<u_int32_t> > weak_checksums;
    //vector< vector<u_int64_t> > strong_checksums;
    weak_checksums=read_vector< vector<u_int32_t> >();
    //strong_checksums = read_vector< vector<u_int64_t> >();
    for(int i=0;i<weak_checksums.size();i++)  {
        vector<u_int32_t> chunk=weak_checksums[i];
        for(int j=0;j<chunk.size();j++){
            cout<<chunk[j]<<endl;
        }
    }
  //  this->send_indexes(unmatchedHashIndex);
boost::system::error_code err;
    cout<<"Recieving unmatched chunk data..."<<endl;
    this->read_chunk_data(err,filepath);
    cout<<"Unmatched chunk data recieved successfully!"<<endl << endl;

//    vector < vector<bool> > unmatchedblockindex=findBlockMatch(weak_checksums,strong_checksums,filepath);
//    for(int i=0 ;i<unmatchedblockindex.size();i++){
//        vector<bool> chunk_blocks=unmatchedblockindex[i];
//        vector<ChunkDat> blocks;
//        for(int j=0;j<chunk_blocks;j++){
//            if(chunk_blocks[j]){
//                blocks.push_back(this->read_block());
//            }
//        }
//        updatefile(blocks,i,j,filepath);
//    }
}

void session::file_created(string filepath){

}



void session::read_chunk_hashes(string filepath)
{
//    cout<<"Recieving chunk hashes..."<<endl;
//    char response[max_length];
//    char size_buf[max_length];

//    strcpy(response,"OK");
    boost::system::error_code err;
//    boost::asio::streambuf s_buf;
//    int size;
//    string input;
//    boost::asio::read_until(socket_,s_buf,"\r\n");
//    istream is(&s_buf);
//    //cout<<"BUffer size after read:"<<s_buf.size()<<endl;
//    is>>size;
//    //cout<<"Got the size as:"<<size<<endl;
//    //cout<<"Got data with size:"<<data_until<<endl;
//    s_buf.consume(2);
//    //cout<<"After consumption:"<<s_buf.size()<<endl;
//    if(s_buf.size()==size){
//        //  cout<<"Wont go!!!!\n";
//        is.read(size_buf,s_buf.size());
//        //  cout<<"Got excess data:"<<size_buf<<endl;
//        input=size_buf;
//    }
//    else{
//        //  size_t got_size = socket_.read_some(boost::asio::buffer(size_buf,size),err);
//        if(s_buf.size()>0){
//            size-=s_buf.size();
//            is.read(size_buf,s_buf.size());
//            input=size_buf;
//            //    cout<<"Input has value:"<<input<<endl;
//        }
//        char get_str[size];
//        size_t got_size = boost::asio::read(socket_,boost::asio::buffer(get_str,size),err);
//        string temp=get_str;
//        temp=temp.substr(0,got_size);
//        input+=temp;

//        if(err)
//            cout<<"Error occurred after reading:"<<get_str<<" and error is:"<<err.message()<<endl;
//    }
//    boost::asio::write(socket_,boost::asio::buffer(response));


//    if(!err){
//        std::istringstream archive_stream_input(input);
//        boost::archive::text_iarchive archive_in(archive_stream_input);
//        archive_in >> chunk_hashes;
//    }
    chunk_hashes=read_vector<u_int64_t>() ;
    cout<<"Chunk hashes recieved successfully!"<<endl;

    cout<<"Finding unmatched hashes..."<<endl;
    vector<bool> unmatchedHashIndex = findMatch(chunk_hashes);
    cout<<"Sending unmatched hash indexes..."<<endl;

    this->send_indexes(unmatchedHashIndex);

    cout<<"Recieving unmatched chunk data..."<<endl;
    this->read_chunk_data(err,filepath);
    cout<<"Unmatched chunk data recieved successfully!"<<endl << endl;


}

template<class T>
vector<T> session::read_vector(){
    char response[max_length];
    char size_buf[max_length];

    strcpy(response,"OK");
    boost::system::error_code err;
    boost::asio::streambuf s_buf;
    int size;
    vector<T> v;
    string input;
    boost::asio::read_until(socket_,s_buf,"\r\n");
    istream is(&s_buf);
    is>>size;
    s_buf.consume(2);
    if(s_buf.size()==size){
        is.read(size_buf,s_buf.size());
        input=size_buf;
    }
    else{
        if(s_buf.size()>0){
            size-=s_buf.size();
            is.read(size_buf,s_buf.size());
            input=size_buf;
        }
        char get_str[size];
        size_t got_size = boost::asio::read(socket_,boost::asio::buffer(get_str,size),err);
        string temp=get_str;
        temp=temp.substr(0,got_size);
        input+=temp;
        if(err)
            cout<<"Error occurred after reading:"<<get_str<<" and error is:"<<err.message()<<endl;
    }
    boost::asio::write(socket_,boost::asio::buffer(response));


    if(!err){
        std::istringstream archive_stream_input(input);
        boost::archive::text_iarchive archive_in(archive_stream_input);
        archive_in >> v;
    }
    return v;
}

template<class T>
void session::send_vector(vector<T> v){
    char res[max_length];
    boost::system::error_code r;
    string d;
    ostringstream a;
    boost::archive::text_oarchive b(a);
    b << v;
    d = a.str();
    boost::asio::streambuf s;
    ostream o(&s);
    o<<d.size()<<"\r\n"<<d;
    boost::asio::write(socket_,s,r);
    if(r)
        cout<<r.message()<<endl ;
    socket_.read_some(boost::asio::buffer(res),r);
}


void session::send_indexes(vector<bool> indexes)
{
    cout<<"Sending unmatched chunk data\n";
    char res[max_length];
    boost::system::error_code err;
    string outbound_data_;
    std::ostringstream archive_stream_output;
    boost::archive::text_oarchive archive_out(archive_stream_output);
    archive_out << indexes;
    outbound_data_ = archive_stream_output.str();
    //  cout<<"Sending unmatched data size:"<<outbound_data_.size()<<endl;
    boost::asio::streambuf sendbuf;
    ostream os(&sendbuf);
    os<<outbound_data_.size()<<"\r\n"<<outbound_data_;

    boost::asio::write(socket_,sendbuf,err);
    if(!err){
        ;
    }
    else
        cout<<err.message()<<endl;
    socket_.read_some(boost::asio::buffer(res),err);
    //  cout<<"Got unmatched chunk data response\n";
}


void session::read_chunk_data(const boost::system::error_code &err,string filepath)
{
    boost::system::error_code ec;
    if(!err){
        int file_wsize=0;
        int index=0;

        char response[max_length];
        strcpy(response,"OK");

        //socket_.read_some(boost::asio::buffer(data_),err);
        socket_.read_some(boost::asio::buffer(data_),ec);
        string size=data_;
        int max_data=1;
        size_t got_size;
        //  int file_rec=0;
        int file_size=boost::lexical_cast<int>(size);
        int chunk_count=file_size;
        cout<<"Getting chunks: "<<file_size<<endl;
        while(file_size>0){
            ChunkDat s;
            char datgot[5];

            int chunksize=0;
            boost::asio::streambuf rddata;


            boost::asio::read(socket_,boost::asio::buffer(datgot,5),ec);

            // cout<<datgot<<endl;

            if(datgot[0]!='A' || datgot[4]!='d'){
                cout<<"Protocol failed\n";
                return;
            }
            //cout<<"Got chunk size in bytes as:"<<(int)datgot[3]<<" Sec: "<<(int)datgot[2]<<" Thirsd: "<<datgot[1]<<endl;
            chunksize|=datgot[1] & 0xFF;
            chunksize<<=16;
            chunksize|=datgot[2] & 0xFF;
            chunksize<<=8;
            chunksize|=datgot[3] & 0xFF;

            //cout<<"Got chunk size as:"<<chunksize<<endl;
            //got_size=boost::asio::read(socket_,rddata.prepare(chunksize),ec);
            got_size=boost::asio::read(socket_,boost::asio::buffer(s.data,chunksize),ec);
            //  got_size=socket_.read_some(boost::asio::buffer(data_),ec);
            cout<<"Got chunk:"<<index+1<<" of "<<chunk_count<<" with size:"<<got_size<<endl;
            rddata.commit(chunksize);


            s.chunk_size=got_size;
            dt.push_back(s);

            file_wsize+=got_size;

            file_size-=max_data;
            index++;
        }

        createFile(chunk_hashes,dt,filepath);
        dt.clear();
        filechunks.clear();
        chunk_hashes.clear();
    }
    else{
        cout<<"Error occurred with :"<<err.message()<<endl;
    }

}




void session::receive_file(string filepath, string name)
{
    std::cout<<"File creation code executing"<<std::endl;
    boost::array<char, 1024> buf;
    boost::system::error_code error;
    std::string path= watch_dir+filepath+name;
    cout<<"To be created as "<<path<<endl;
    std::ofstream output_file(path.c_str(), std::ios_base::binary);

    if (!output_file)
    {
        std::cout << "failed to open " << name << std::endl;
    }
    for (;;)
    {
        size_t len = socket_.read_some(boost::asio::buffer(buf), error);
        if (len>0)
            output_file.write(buf.c_array(), (std::streamsize)len);
        if (error)
        {
            std::cout << error.message() << std::endl;
            break;
        }
    }
    std::cout << "Received file with " << output_file.tellp() << " bytes.\n";
}


