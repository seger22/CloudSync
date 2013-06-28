#include "session.h"

session::session(boost::asio::io_service &io_service):socket_(io_service)
{
    watch_dir="/home/satthy/Server";
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
          string sourcepath;
          bool matched = findFileChecksum(strs.at(3),sourcepath);

 char res[5];
if(matched)
{
    cout << endl << "Same file exists in the Server" << endl;
    strcpy(res,"M");
    boost::asio::write(socket_,boost::asio::buffer(res));
copyFile(sourcepath,watch_dir+strs.at(1)+strs.at(2));
cout << "File has been copied successfully!" << endl << endl;
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
          arg1=strs.at(1).c_str();
          arg2=strs.at(2).c_str();
          this->read_chunk_hashes(strs.at(1)+strs.at(2));

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


 void session::read_chunk_hashes(string filepath)
 {
     cout<<"Recieving chunk hashes..."<<endl;
     char response[max_length];
     int max_hash=49;
     char size_buf[max_length];

     vector<int> unmatchedHashes;
     strcpy(response,"OK");
     boost::system::error_code err;     
     socket_.read_some(boost::asio::buffer(data_),err);
    string size=data_;
     int file_size=boost::lexical_cast<int>(size);

     if(!err){
         do{
             boost::this_thread::sleep(boost::posix_time::seconds(1));
             size_t got_size=socket_.read_some(boost::asio::buffer(size_buf),err);
             boost::asio::write(socket_,boost::asio::buffer(response));

               string input=size_buf;
               input=input.substr(0,got_size);

             if(!err){
               std::istringstream archive_stream_input(input);
                boost::archive::text_iarchive archive_in(archive_stream_input);
                archive_in >> filechunks;

                chunk_hashes.insert(chunk_hashes.end(),filechunks.begin(),filechunks.end());

                filechunks.clear();
             }
             file_size-=max_hash;


         }while(file_size>=0);
          cout<<"Chunk hashes recieved successfully!"<<endl;
           cout<<"Finding unmatched hashes..."<<endl;
         unmatchedHashes=findMatch(chunk_hashes);

 cout<<"Sending unmatched hash indexes..."<<endl;
         string size_d=boost::lexical_cast<string>(unmatchedHashes.size());
         strcpy(response,size_d.c_str());
         boost::asio::write(socket_,boost::asio::buffer(response),err);
         int count=unmatchedHashes.size();
         int max_hashes=200;
         vector<int> hashvector;
         int endnum,startnum=0;
         while(count>0){

             endnum= startnum+((count>=max_hashes) ? max_hashes : count);
         for(int i=startnum;i<endnum;i++){
           hashvector.push_back(unmatchedHashes[i]);
        }
             this->send_indexes(hashvector);
         hashvector.clear();
            count-=max_hashes;
            startnum+=max_hashes;
         }

          cout<<"Recieving unmatched chunk data..."<<endl;
        this->read_chunk_data(err,filepath);
cout<<"Unmatched chunk data recieved successfully!"<<endl << endl;
     }
     else
     cout<<"Error reading data"<<endl;
 }

 void session::send_indexes(vector<int> indexes)
 {
     char res[max_length];
     boost::system::error_code err;
     string outbound_data_;
      std::ostringstream archive_stream_output;
      boost::archive::text_oarchive archive_out(archive_stream_output);
      archive_out << indexes;
      outbound_data_ = archive_stream_output.str();

      std::vector<boost::asio::const_buffer> buffers;
      buffers.push_back(boost::asio::buffer(outbound_data_));

      boost::asio::write(socket_,buffers,err);
      if(!err){
         ;
      }
      else
          cout<<err.message()<<endl;
      socket_.read_some(boost::asio::buffer(res),err);

 }



 vector<int> session::get_unmatched_hashes(string data){


     std::istringstream archive_stream_input(data);
     boost::archive::text_iarchive archive_in(archive_stream_input);
     archive_in >> filechunks;

     vector<int> unmatchedIndeces= findMatch(filechunks);
     filechunks.clear();

     return unmatchedIndeces;
 }




 void session::send_unmatched_hashes(const boost::system::error_code &err)
 {
     if(!err){
         boost::system::error_code error;
       //  cout<<data_<<endl;
         cout<<"send_unmatched_hashes"<<endl;
       //  vector<u_int64_t> t;
vector<int> unmatchedIndeces= findMatch(filechunks);
         std::string outbound_data_;
         std::ostringstream archive_stream_output;
         boost::archive::text_oarchive archive_out(archive_stream_output);
         archive_out << unmatchedIndeces;
         outbound_data_ = archive_stream_output.str();

         std::vector<boost::asio::const_buffer> buffers;
         buffers.push_back(boost::asio::buffer(outbound_data_));
         boost::asio::write(socket_,buffers,error);
         for(int k=0;k<unmatchedIndeces.size();k++){
             cout<<"Reading chunk header and data: "<<k<<endl;

         }
     }
     else
         cout<<"Could not send chunks "<<err.message()<<endl;
 }


 void session::read_chunk_data_header(const boost::system::error_code &err){
    cout<<"Reading chunk header"<<endl;
     boost::system::error_code error;
    // boost::asio::async_read_until(socket_,sbuf,'\n',boost::bind(&session::read_chunk_data,this,boost::asio::placeholders::error()));
     boost::asio::read_until(socket_,sbuf,'\n',error);
//     this->read_chunk_data(error);
     this->temp_read_chunk_data();
 }

 void session::temp_read_chunk_data()
 {
     size_t file_size;
     cout<<"Reading header buffer"<<endl;
     std::istream request_stream(&sbuf);
           request_stream >> file_size;
           cout<<"File size is :"<<file_size<<endl;
 }






void session::read_chunk_data(const boost::system::error_code &err,string filepath)
 {

    boost::system::error_code ec;
     if(!err){
         int file_wsize=0;
         int index=0;
         vector<int> got_size_vector;
         string in;


       //  boost::asio::streambuf buf;
         char response[max_length];
         strcpy(response,"OK");

         //socket_.read_some(boost::asio::buffer(data_),err);
         socket_.read_some(boost::asio::buffer(data_),ec);
        string size=data_;
        int max_data=1;

         int file_size=boost::lexical_cast<int>(size);

         while(file_size>0){
             ChunkDat s;
             boost::this_thread::sleep(boost::posix_time::seconds(1));
             size_t got_size=socket_.read_some(boost::asio::buffer(s.data),ec);
             s.chunk_size=got_size;
             dt.push_back(s);

             boost::asio::write(socket_,boost::asio::buffer(response));

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


void session::handle_write_request(const boost::system::error_code &err)
 {
      if(err)
      {
          std::cout<<"Oh no something wnet wrong :("<<std::endl;
      }
      else
      {
          cout<<"Message sent successfully!!!";
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


