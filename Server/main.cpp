#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    boost::asio::io_service io_service_;
    short port=9999;
    server s(io_service_,port);
    io_service_.run();
}
