//
/// \file fcgitestclient.cpp
/// \author James Marlowe
/// \brief A worker that registers with tcp router and responds to requests inside fcgi loop
//

#include <stdlib.h>
#include <iostream>
#include <string>
#include "zmq/zmqcpp.h"
#include "fcgio.h"

using namespace std;

int main(void)
{
    /////////////////////////////// fcgi setup ///////////////////////////////
    // Backup the stdio streambufs
    std::streambuf *cin_streambuf  = std::cin.rdbuf();
    std::streambuf *cout_streambuf = std::cout.rdbuf();
    std::streambuf *cerr_streambuf = std::cerr.rdbuf();
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest (&request, 0, 0);
    /////////////////////////////// zmq setup ////////////////////////////////
    zmqcpp::Socket sock(ZMQ_STREAM);
    std::cout << "Connecting to server…" << std::endl;
    sock.connect("tcp://localhost:5555");
    sock._conn();
    uint8_t id [256];
    size_t id_size = 256;
    zmq_getsockopt ((void*)sock.raw_sock(), ZMQ_IDENTITY, id, &id_size);

    while (FCGX_Accept_r (&request) == 0)
    {
        std::string send = "test request";
        std::cout << "Content-type: text/html\r\n\r\n Client sending : " << send << std::endl;
        sock.raw_sock().send(id, id_size, ZMQ_SNDMORE);
        sock.raw_sock().send((void*)send.c_str(), send.size());
        sock.raw_sock().recv(id, 256);
        zmqcpp::Message msg;
        sock.recv(msg);
        std::cout << "Content-type: text/html\r\n\r\n Client received : " << msg.last() << std::endl;
    }
    /////////////////////////////// cleanup ///////////////////////////////
    // restore stdio streambufs
    std::cin.rdbuf (cin_streambuf);
    std::cout.rdbuf (cout_streambuf);
    std::cerr.rdbuf (cerr_streambuf);
    return 0;
}
