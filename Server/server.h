#ifndef SERVER_H
#define SERVER_H

#include<boost/asio.hpp>
#include<session.h>
#include<boost/thread.hpp>

using namespace std;
using boost::asio::ip::tcp;

class server
{
public:
    server(boost::asio::io_service& io_service, short port);
    void handle_accept(session* new_session,const boost::system::error_code& error);
private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

#endif // SERVER_H
