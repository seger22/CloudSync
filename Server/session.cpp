
#include "session.h"

session::session(boost::asio::io_service &io_service):socket_(io_service)
{  
    watch_dir=ConfigurationManager::getLocation();

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



/**
 * @brief session::read_request
 * @param error
 */
void session::read_request(const boost::system::error_code &error){
    try{
        if (!error)
        {
            while(true){
                //  cout<<"read_request"<<endl;
                boost::system::error_code err;
                char requestString[max_length]={0};
                bool errorflag=this->read_request_from_client(requestString);
                // this->read_from_client(requestString);

                if(errorflag){
                    cout<<"Client has stopped, so session stopping"<<endl;
                    return;
                }

               // cout<<"Read data: \n"<<requestString<<endl;
                // cout<<data_<<endl;

                std::vector<std::string> strs;
                boost::split(strs, requestString, boost::is_any_of("|"));


                const char* req=strs.at(0).c_str();
                if(strcmp(req,"fc")==0){
                    std::cout<<"File create command received"  <<std::endl;
                    string fullfilepath=(watch_dir+strs.at(1)+ strs.at(2)).c_str();
                    bool matched = findFileChecksum(strs.at(3),fullfilepath);

                    char res[5]={0};
                    if(matched)
                    {
                        cout<<"File already exists here, so copied to the destination!\n";
                        strcpy(res,"M");
                        this->send_integer(1);
                        boost::asio::write(socket_,boost::asio::buffer(res,1));
                    }
                    else{
                        bool fileexist= filehandler_.file_exists(fullfilepath);
                        if(fileexist && strcmp(strs.at(4).c_str(),"1")!=0){
                            cout<<"File with same name already exists here, so block matching also will be done!\n";
                            strcpy(res,"E");
                            this->send_integer(1);
                            boost::asio::write(socket_,boost::asio::buffer(res,1));
                        }
                        else{
                            strcpy(res,"U");
                            this->send_integer(1);
                            boost::asio::write(socket_,boost::asio::buffer(res,1));
                            this->file_created(strs.at(1),strs.at(2));
                        }
                    }
                }

                else if(strcmp(req,"sf")==0){
                    cout<<"Small file sending command recevied\n";
                    this->write_small_file(strs.at(1),strs.at(2));
                }

                else if(strcmp(req,"fm")==0)
                {
                    char res[5]={0};
                    std::cout<<"File modified command received"  <<std::endl;
                     string fullfilepath=(watch_dir+strs.at(1)+ strs.at(2)).c_str();
                    bool matched = findFileChecksum(strs.at(3),fullfilepath);
                    if(matched){
                        this->send_integer(1);
                         strcpy(res,"M");
                         cout<<"File already exists in the destination!!\n";
                         boost::asio::write(socket_,boost::asio::buffer(res,1));
                    }
                    else{
                        this->send_integer(1);
                        strcpy(res,"U");
                        boost::asio::write(socket_,boost::asio::buffer(res,1));
                        this->file_modified(strs.at(1),strs.at(2));
                    }


                }
                else if(strcmp(req,"dc")==0)
                {
                    std::cout<<"Directory create command received"  <<std::endl;
                    filehandler_.create_directory(strs.at(1).c_str(),strs.at(2).c_str());
                }

                else if(strcmp(req,"dd")==0){
                    std::cout<<"Directory delete command received"<<std::endl;
                    filehandler_.delete_directory(strs.at(1).c_str(),strs.at(2).c_str());
                }
                else if(strcmp(req,"dr")==0){
                    std::cout<<"Directory rename command received"<<std::endl;
                    filehandler_.rename_directory(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
                }

                else if(strcmp(req,"fr")==0){
                    std::cout<<"File rename command received"<<std::endl;
                    filehandler_.rename_file(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
                }

                else if(strcmp(req,"fv")==0){
                    std::cout<<"File move command received"<<std::endl;
                    filehandler_.move_file(strs.at(1).c_str(),strs.at(2).c_str(),strs.at(3).c_str());
                }

                else if(strcmp(req,"dm")==0){
                    std::cout<<"Directory move command received"<<std::endl;
                    filehandler_.move_directory(strs.at(1).c_str(),strs.at(2).c_str());
                }

                else if(strcmp(req,"fd")==0){
                    std::cout<<"File delete command received"<<std::endl;
                    filehandler_.delete_file(strs.at(1).c_str(),strs.at(2).c_str());
                }

                //strcpy(data_,"");
                // this->read_request(err);
            }
        }
        else
            cout<<"Oh No Something went wrong\n";
    }catch(const char *Ex){
        cout<<"Connection is closed because of:"<<Ex<<endl;
        return;
    }
}

//
void session::file_created(string filepath,string filename){
    cout<<"-----------------------------Chunk Matching phase----------------------------\n\n";
    cout<<"Receiving chunk hashes...\n";
    chunk_hashes=this->read_chunk_hashes();
    cout<<"Total chunk hashes received: "<<chunk_hashes.size()<<endl;

    if(chunk_hashes.size()==0) return;
    vector<Chunk> chunks;
    bool tempbool;
    int matchcount=0;
    vector<bool> unmatchedHashIndex = findChunkMatch(chunk_hashes,&chunks,&tempbool,matchcount);

    cout<<"Sending unmatched chunk hash indexes...\n";
    send_vector<bool>(unmatchedHashIndex);

    this->read_chunk_data(filepath,filename,chunk_hashes.size()-matchcount);
    cout<<"Unmatched chunk data recieved successfully!\n\n";
}



/**
 * @brief session::file_modified
 * @param filepath
 */
//
void session::file_modified(string filepath,string filename){
    cout<<"-----------------------------Chunk Matching phase----------------------------\n\n";
    cout<<"\nReceiving chunk hashes...\n";
    chunk_hashes=this->read_chunk_hashes();
    cout<<"Total chunk hashes received: "<<chunk_hashes.size()<<endl;
    if(chunk_hashes.size()==0) return;
    vector<Chunk> chunks;
    bool allmatched=false;
    int matchcount=0;
    vector<bool> unmatchedHashIndex = findChunkMatch(chunk_hashes,&chunks,&allmatched,matchcount);
    if(allmatched){
        cout<<"Same file in the destination!!!\n";
        send_vector<bool>(unmatchedHashIndex);
        return;
    }
    cout<<"\nSending unmatched chunk hash indexes...\n\n";
    send_vector<bool>(unmatchedHashIndex);
    cout<<"-----------------------------Block Matching phase----------------------------\n\n";
    cout<<"\nReceiving block hashes...\n\n";
    vector<BlockChecksumSerial> block_checksums=this->read_block_hashes();
    cout<<"Total number of checksums received: "<<block_checksums.size()<<endl;

    vector<int> matchedOffsets;
    vector<int> chunkOffsets;
    vector<int> all_block_Offsets;
    cout<<"Finding matching blocks...\n";
    vector <bool> matchedBlockIndexes = findBlockMatch(block_checksums,watch_dir+filepath+filename,&matchedOffsets);
    cout<<"\nSending unmatched block indexes...\n";
    send_vector<bool>(matchedBlockIndexes);
    cout<<"Getting chunk and block offsets...\n";
    vector<int> lengths;
    chunkOffsets=this->read_num_vector();
    all_block_Offsets=this->read_num_vector();
    vector<ChunkBlockOrderer> chunk_block_offsets=mergeOffsets(chunkOffsets,all_block_Offsets,&lengths);
    cout<<"Got "<<lengths.size()<<" lengths\n";
    cout<<"Recieving unmatched block data...\n";
    this->read_block_data(chunks,chunk_block_offsets,matchedOffsets,lengths,watch_dir+filepath,filename);
    cout<<"Unmatched block data recieved successfully!\n\n";
}



vector<ChunkBlockOrderer> session::mergeOffsets(vector<int> chunkOffsets, vector<int> blockOffsets, vector<int> *lengths){
    vector<ChunkBlockOrderer> chunk_blocks;   
    int index;
    int prevOffset=-1;
    bool isBlock=false;
    int chunkorder=0, blockorder=0;
    //for(int i=0;i<chunkOffsets.size()+blockOffsets.size();i++){
    while(chunkorder<chunkOffsets.size() && blockorder+1<blockOffsets.size()){
        if(chunkOffsets[chunkorder]>blockOffsets[blockorder]){
            ChunkBlockOrderer block(blockOffsets[blockorder],false);
            chunk_blocks.push_back(block);
            if(isBlock){
                lengths->push_back(blockOffsets[blockorder]-prevOffset);
            }
            prevOffset=blockOffsets[blockorder];
            isBlock=true;
            blockorder++;
        }
        else{
            // cout<<"Chunk offset"<<chunkOffsets[chunkorder] <<"added!!\n";
            ChunkBlockOrderer chunk(chunkOffsets[chunkorder],true);
            chunk_blocks.push_back(chunk);            
            if(isBlock){
                lengths->push_back(chunkOffsets[chunkorder]-prevOffset);
            }
            isBlock=false;
            chunkorder++;
        }

    }
    if(blockorder+1<blockOffsets.size()){
        while(blockorder+1<blockOffsets.size()){
            ChunkBlockOrderer block(blockOffsets[blockorder],false);
            chunk_blocks.push_back(block);            
            if(isBlock)
                lengths->push_back(blockOffsets[blockorder]-prevOffset);
            prevOffset=blockOffsets[blockorder];
            isBlock=true;
            blockorder++;
        }
    }
    else if(chunkorder<chunkOffsets.size()){
        while(chunkorder<chunkOffsets.size()){
            //  cout<<"Chunk offset"<<chunkOffsets[chunkorder] <<"added!!\n";
            ChunkBlockOrderer chunk(chunkOffsets[chunkorder],true);
            chunk_blocks.push_back(chunk);
            if(isBlock){
                lengths->push_back(chunkOffsets[chunkorder]-prevOffset);
            }
            chunkorder++;
            isBlock=false;
        }
    }

    if(isBlock)
        lengths->push_back(blockOffsets[blockorder]-prevOffset);

    return chunk_blocks;
}



//this->read_block_data(chunks,chunk_block_offsets,matchedOffsets,watch_dir+filepath,filename);
//
void session::read_block_data(vector<Chunk> chunks, vector<ChunkBlockOrderer> chunk_blocks,vector<int> matched_blocks,vector<int> lengths,string filepath, string filename){
    //vector<bool> blockmatchData, vector<int> matchedOffsets, vector<int> all_block_offsets,string filepath,string filename
    boost::system::error_code ec;
    int file_wsize=0;
    int index=0;
    vector<ChunkDat> block;
    char response[max_length];
    strcpy(response,"OK");
    char blocksize[10]={0};
    //socket_.read_some(boost::asio::buffer(data_),ec);
    this->read_from_client(blocksize);
    string size=blocksize;
    int max_data=1;
    size_t got_size;
    int file_size=boost::lexical_cast<int>(size);
    //  int chunk_count=file_size;
    //  cout<<"Getting chunks: "<<file_size<<endl;
    while(file_size>0){
        ChunkDat s;
        char datgot[3]={0};

        int chunksize=0;
        boost::asio::streambuf rddata;
        boost::asio::read(socket_,boost::asio::buffer(datgot,3),ec);

//        if(datgot[0]!='A' || datgot[4]!='d'){
//            cout<<"Protocol failed\n";
//            return;
//        }
        //cout<<"Got chunk size in bytes as:"<<(int)datgot[3]<<" Sec: "<<(int)datgot[2]<<" Thirsd: "<<datgot[1]<<endl;
        chunksize|=datgot[0] & 0xFF;  //0xFF least significant byte
        chunksize<<=8;
        chunksize|=datgot[1] & 0xFF;
        chunksize<<=8;
        chunksize|=datgot[2] & 0xFF;

        //cout<<"Got chunk size as:"<<chunksize<<endl;
        //got_size=boost::asio::read(socket_,rddata.prepare(chunksize),ec);
        got_size=boost::asio::read(socket_,boost::asio::buffer(s.data,chunksize),ec);
        //  got_size=socket_.read_some(boost::asio::buffer(data_),ec);
        //      cout<<"Got chunk:"<<index+1<<" of "<<chunk_count<<" with size:"<<got_size<<endl;
        //    cout<<"Got block: "<<index+1<<" of "<<chunk_count<<endl;
        rddata.commit(chunksize);


        s.chunk_size=got_size;
        block.push_back(s);

        file_wsize+=got_size;

        file_size-=max_data;
        index++;
    }
    cout<<"Total blocks received: "<<index<<endl;
    writeData(chunks,block,matched_blocks,chunk_blocks,lengths,filename,filepath);
    // writeBlocks(dt,matchedOffsets,all_block_offsets,filepath,filename);
    //createFileWithBlocks(blockmatchData,matchedOffsets,all_block_offsets,dt,filepath,filename);
    block.clear();
}

/**
 *Receive any vector from the client
 */
//
template<class T>
vector<T> session::read_vector(){
    char response[max_length]={0};
    char size_buf[max_length]={0};

    strcpy(response,"OK");
    boost::system::error_code err;
    boost::asio::streambuf s_buf;
    int size=0;
    vector<T> v;
    string input="";
    boost::asio::read_until(socket_,s_buf,"\r\n",err);
    if(err){
        //cout<<"Error occurred: "<<err.message()<<" and system is reset\n";
        throw "Vector size read error\n";
    }
    istream is(&s_buf);
    is>>size;
    s_buf.consume(2);
    if(s_buf.size()==size){
        cout<<"Got as excess data\n";
        is.read(size_buf,s_buf.size());
        input=size_buf;
        input=input.substr(0,size);
    }
    else{
        if(s_buf.size()>0){
            size-=s_buf.size();
            is.read(size_buf,s_buf.size());
            input=size_buf;
        }
        char get_str[size];
        size_t got_size = boost::asio::read(socket_,boost::asio::buffer(get_str,size),err);
        // cout<<"Got size: "<<got_size<<endl;
        string temp=get_str;
        temp=temp.substr(0,got_size);
        input+=temp;
        if(err){
            cout<<"Error occurred after reading: "<<get_str<<" and error is: "<<err.message()<<endl;
            throw "Vector read error!!";
        }
    }
    boost::asio::write(socket_,boost::asio::buffer(response));


    if(!err){
        try{
            std::istringstream archive_stream_input(input);
            boost::archive::text_iarchive archive_in(archive_stream_input);
            archive_in >> v;
        }catch(boost::archive::archive_exception &ex){
            //cout<<"Error occurred from the receiving vector part. Details:\n"<<ex.what()<<endl;
            throw "Boost archive Exception";
        }
    }
    return v;
}

/**
 *Send any vector to the client
 */
//
template<class T>
void session::send_vector(vector<T> v){
    // char res[max_length];
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
    if(r){
        cout<<r.message()<<endl ;
        throw "Send error!!\n";
    }
    this->read_from_client(data_);
    //socket_.read_some(boost::asio::buffer(res),r);
}

void session::write_small_file(string filepath,string filename){
    char datgot[3]={0};
    boost::system::error_code ec;
    boost::asio::read(socket_,boost::asio::buffer(datgot,3),ec);
    int chunksize=0;
    int got_size=0;
    ChunkDat s;
    chunksize|=datgot[0] & 0xFF;
    chunksize<<=8;
    chunksize|=datgot[1] & 0xFF;
    chunksize<<=8;
    chunksize|=datgot[2] & 0xFF;

    got_size=boost::asio::read(socket_,boost::asio::buffer(s.data,chunksize),ec);
    cout<<"Got file with size:"<<chunksize<<endl;
    s.chunk_size=got_size;
    writeChunkFile(s,watch_dir+filepath+filename);
}


//
void session::read_from_client(char *p){
    boost::system::error_code err;
    char datgot[2]={0};
    int chunksize=0;
    strcpy(p,"");
    boost::asio::read(socket_,boost::asio::buffer(datgot,2),err);
    if(err) throw "Invalid response\n";
    chunksize|=datgot[0] & 0xFF;
    chunksize<<=8;
    chunksize|=datgot[1] & 0xFF;
    boost::asio::read(socket_,boost::asio::buffer(p,chunksize),err);
    if(err) throw "Invalid response\n";
}


//
bool session::read_request_from_client(char *p){
    boost::system::error_code err;
    char datgot[2]={0};
    int chunksize=0;
    strcpy(p,"");
    boost::asio::read(socket_,boost::asio::buffer(datgot,2),err);
    if(err)
        return true;

    chunksize|=datgot[0] & 0xFF;
    chunksize<<=8;
    chunksize|=datgot[1] & 0xFF;
    boost::asio::read(socket_,boost::asio::buffer(p,chunksize),err);
    if(err)
        return true;
    return false;
}


/**
 * @brief session::read_chunk_data -
 * @param err
 * @param filepath
 */
//
void session::read_chunk_data(string filepath, string filename,int unmatchcount)
{
    boost::system::error_code ec;
    int index=0;
    char response[max_length]={0};
    strcpy(response,"OK");
    size_t got_size;
    int file_size=unmatchcount;

    int chunk_count=unmatchcount;
    cout<<"Getting chunks: "<<file_size<<endl;

    try{
      //  boost::thread chunkwriter(writeChunks,chunk_hashes,&chunkqueue,filepath,filename);
      //  cout<<"Thread successfully created\n";

        while(file_size>0){
            ChunkDat s;
            char datgot[3]={0};

            int chunksize=0;

            boost::asio::read(socket_,boost::asio::buffer(datgot,3),ec);
            //boost::asio::transfer_exactly()

//            if(datgot[0]!='A' || datgot[4]!='d'){
//                cout<<"Protocol failed\nMessage:"<<datgot;
//                throw "Invalid Message Format\n";
//            }

            chunksize|=datgot[0] & 0xFF;
            chunksize<<=8;
            chunksize|=datgot[1] & 0xFF;
            chunksize<<=8;
            chunksize|=datgot[2] & 0xFF;

            got_size=boost::asio::read(socket_,boost::asio::buffer(s.data,chunksize),ec);

           // cout<<"Got chunk:"<<index+1<<" of "<<chunk_count<<" with size:"<<got_size<<" : "<<(int)s.data[0]<<" : "<<(int)s.data[chunksize-1]<<endl;

            s.chunk_size=got_size;
          //  if(chunkqueue.size()>15){while(chunkqueue.size()>15){}}
            chunkqueue.push(s);
            //file_wsize+=got_size;

            file_size--;//=max_data;
            index++;
        }

        //chunkwriter.join();
    }catch(boost::thread_exception &e){
        cout<<"Thread Exception thrown: "<<e.what()<<endl;
    }
    catch(exception &e){
        cout<<"Exception thrown:"<<e.what()<<endl;
    }
    writeChunks(chunk_hashes,&chunkqueue,filepath,filename);
    // createFile(chunk_hashes,block,filepath+"/"+filename);
  //  block.clear();
    filechunks.clear();
    chunk_hashes.clear();

}

vector<u_int64_t> session::read_chunk_hashes(){
    int count=this->read_integer();
    vector<u_int64_t> chunk_hashes;
    for(int i=0;i<count;i++){
        chunkhash hashbytes;

        boost::asio::read(socket_,boost::asio::buffer(hashbytes.data,8),boost::asio::transfer_exactly(8));

        u_int64_t hash=0;
        for(int j=0;j<8;j++){
            hash |= hashbytes.data[j] & 0xFF;
            if(j+1<8)
                hash <<= 8;
        }


     //   cout<<"\nHash obtained:"<<hash<<endl;
        chunk_hashes.push_back(hash);
    }
    return chunk_hashes;
}



vector<int> session::read_num_vector(){
    int count=this->read_integer();
    vector<int> numbers;
    int num;
    for(int i=0;i<count;i++){
       num=this->read_integer();
       numbers.push_back(num);
    }
    return numbers;
}

unsigned char* session::read_buffer(int buf_size){
    unsigned char buf[buf_size];
    boost::asio::read(socket_,boost::asio::buffer(buf,buf_size));
    return buf;
}

unsigned int session::read_integer(){
    unsigned int num=0;
    unsigned char bytes[4]={0};
    boost::asio::read(socket_, boost::asio::buffer(bytes,4));
    num|=bytes[0] & 0xFF;
    num<<=8;
    num|=bytes[1] & 0xFF;
    num<<=8;
    num|=bytes[2] & 0xFF;
    num<<=8;
    num|=bytes[3] & 0xFF;
    return num;
}


void session::send_integer(unsigned int n){
    unsigned char bytes[4];
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    boost::asio::write(socket_,boost::asio::buffer(bytes),boost::asio::transfer_exactly(4));
}


vector<BlockChecksumSerial> session::read_block_hashes(){
    vector<BlockChecksumSerial> block_hashes;
    unsigned int count=this->read_integer();
    cout<<"Got count as: "<<count<<endl;
    for(unsigned int i=0;i<count;i++){
        unsigned int weeksum = this->read_integer();
        unsigned char* buf=this->read_buffer(sizeof(BlockChecksumSerial::strongsum));
        BlockChecksumSerial block(weeksum,buf);
//        ofstream outfile;
//        outfile.open("/home/mayuresan/Project/Serverlog.txt",ofstream::out|ofstream::app);
//       outfile<<weeksum<<endl;
//       for(int j=0;j<16;j++)
//             outfile<<(int)block.strongsum[j]<<endl;
//        outfile.close();
        block_hashes.push_back(block);
    }
    return block_hashes;
}




/**
 * @brief session::receive_file - Receive a complete file from the client
 * @param filepath - Relative path for the file
 * @param name - Name of the file
 */
void session::receive_file(string filepath, string name)
{
    char response[max_length]={0};
    std::cout<<"File creation code executing"<<std::endl;
    boost::array<char, 1024> buf;
    boost::system::error_code error;
    std::string path= watch_dir+filepath+name;
    cout<<"To be created as "<<path<<endl;
    std::ofstream output_file(path.c_str(), std::ios_base::binary);
    size_t file_size = 0;

    boost::asio::streambuf request_buf;
    boost::asio::read_until(socket_, request_buf, "\n\n");
    std::cout<< "request size:" << request_buf.size() << "\n";
    std::istream request_stream(&request_buf);
    request_stream >> file_size;
    request_stream.read(buf.c_array(), 2); // eat the "\n\n"


    if (!output_file)
    {
        std::cout << "failed to open " << name << std::endl;
    }

    do  {
        request_stream.read(buf.c_array(), (std::streamsize)buf.size());
        std::cout <<"writing " << request_stream.gcount() << " bytes.\n";
        output_file.write(buf.c_array(), request_stream.gcount());
    } while (request_stream.gcount()>0);
    if(output_file.tellp()<(std::fstream::pos_type)(std::streamsize)file_size)
    for (;;)
    {
        size_t len = socket_.read_some(boost::asio::buffer(buf), error);
        if (len>0)
            output_file.write(buf.c_array(), (std::streamsize)len);
        if (output_file.tellp()== (std::fstream::pos_type)(std::streamsize)file_size)
            break; // file was received
        if (error)
        {
            std::cout << error.message() << std::endl;
            break;
        }
    }
    strcpy(response,"OK");
boost::asio::write(socket_,boost::asio::buffer(response,2));
    std::cout << "Received file with " << output_file.tellp() << " bytes.\n";
}


