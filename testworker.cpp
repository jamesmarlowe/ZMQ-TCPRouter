//
/// \file testworker.cpp
/// \author James Marlowe
/// \brief A worker that registers with tcp router and responds to requests
//

#include "zmq/zmqcpp.h"
#include <iostream>

int main (int argc, char *argv[])
{
    std::string base = "tcp://localhost:";
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " worker_port(example: 5556)" << std::endl;
        exit (1);
    }
    zmqcpp::Socket socket (ZMQ_REQ);
    std::cout << "Connecting to server…" << std::endl;
    socket.connect (base + argv[1]);
    std::cout << "Registering with router…" << std::endl;
    socket.send (zmqcpp::Message("__ready"));
    std::cout << "Waiting…" << std::endl;
    while (true)
    {
        zmqcpp::Message request_msg;
        socket.recv (request_msg);
        std::cout << " Worker received : " << request_msg.last() << std::endl;
        zmqcpp::Message reply_msg;
        reply_msg.add_frame (* (request_msg.frames().front()));
        reply_msg.add_frame ("");
        std::string send = "test response";
        reply_msg.add_frame (send);
        std::cout << " Worker sending : " << send << std::endl;
        socket.send (reply_msg);
    }
    return 0;
}
