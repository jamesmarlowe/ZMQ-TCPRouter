//
/// \file testclient.cpp
/// \author James Marlowe
/// \brief call TCP router with test data to confirm working
//

#include <iostream>
#include "zmq/zmqcpp.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    std::string base = "tcp://localhost:";
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " client_port(example: 5555)" << std::endl;
        exit (1);
    }
    zmqcpp::Socket sock(ZMQ_STREAM);
    std::cout << "Connecting to server…" << std::endl;
    sock.connect((base + argv[1]).c_str());
    uint8_t id [256];
    size_t id_size = 256;

    zmq_getsockopt ((void*)sock.raw_sock(), ZMQ_IDENTITY, id, &id_size);

    std::string send = "test request";
    std::cout << " Client sending : " << send << std::endl;
    sock.raw_sock().send(id, id_size, ZMQ_SNDMORE);
    sock.raw_sock().send((void*)send.c_str(), send.size());

    sock.raw_sock().recv(id, 256);
    zmqcpp::Message msg;
    sock >> msg;
    std::cout << " Client received : " << msg.last() << std::endl;

    return 0;
}
