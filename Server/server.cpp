#include "server.h"


server::server(boost::asio::io_service &io_service, short port): io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
    cout << "------Server Started!----------" << endl;
  //  chunk_directory("/home/satthy/fileserver");
  // DirectoryChecksum("/home/satthy/fileserver");
    //cout << "all done!!!!!!!!!" << endl;
    session* new_session = new session(io_service_);
    boost::thread t(boost::bind(&boost::asio::io_service::run,&io_service_));
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
