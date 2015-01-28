//
///file TCPRouter.cpp
///brief A router that takes a TCP stream and sends it to the backend
///author James Marlowe
//

#include <zmq/zmqcpp.h>
#include <sstream>
#include <list>
#include <iostream>

int main (int argc, char *argv[])
{
    std::string base = "tcp://localhost:";
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " router_port(example: 5556)" << std::endl;
        exit (1);
    }
    zmqcpp::Socket socket (ZMQ_REQ);
    std::cout << "Connecting to server…" << std::endl;
    socket.connect (base + argv[1]);
    socket.send (zmq::message_t msg ("__ready", 7));
    while (true)
    {
        zmqcpp::message_t request_msg;
        socket.recv (request_msg);
        zmqcpp::Message reply_msg;
        reply_msg.add_frame (* (request_msg.frames().front()));
        reply_msg.add_frame ("");
        reply_msg.add_frame ("test_data");
        socket.send (reply_msg);
    }
    return 0;
}
