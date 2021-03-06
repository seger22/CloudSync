#include "server.h"


server::server(boost::asio::io_service &io_service, short port): io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
   string allpath=ConfigurationManager::getAllUserFileLocation();
       QString path(QDir::home().path());
     if (path!=NULL){
         path = path +QString(allpath.c_str());
         QDir dir(path);
         if (!dir.exists()) {
             dir.mkpath(".");
         }
     }
   chunk_directory(path.toStdString().c_str());
 //   cout<<"Directory chunking completed!!\n";
   // DirectoryChecksum(path.toStdString().c_str());
     cout << "------Server Started!----------" << endl;
    session* new_session = new session(io_service_);
  //  boost::thread t(boost::bind(&boost::asio::io_service::run,&io_service_));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
}


void server::handle_accept(session *new_session, const boost::system::error_code &error){
    if (!error)
    {
       // cout<<"Creating new session and starting"<<endl;
      new_session->start();
      new_session = new session(io_service_);
      acceptor_.async_accept(new_session->socket(),
          boost::bind(&server::handle_accept, this, new_session,
            boost::asio::placeholders::error));

    }
    else
    {
      delete new_session;
    }
}
