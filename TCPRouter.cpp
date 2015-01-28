//
///file TCPRouter.cpp
///brief A router that takes a TCP stream and sends it to the backend
///author James Marlowe
//

#include <zmq.hpp>
#include <sstream>
#include <list>
#include <iostream>

const int FRONT = 0;
const int BACK = 1;

void fwd_req (zmq::socket_t &f, zmq::socket_t &b, std::list<std::string> &workers);
void fwd_rep (zmq::socket_t &b, zmq::socket_t &f, std::list<std::string> &workers);

int main()
{
    std::string base = "tcp://*:";
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " in_port(example: 5555) out_port(example:5556)" << std::endl;
        exit (1);
    }
    zmq::context_t ctx (1);
    zmq::socket_t front (ctx, ZMQ_STREAM);
    zmq::socket_t back (ctx, ZMQ_ROUTER);
    zmq_pollitem_t pollers[] =
    {
        { (void *)front, 0, ZMQ_POLLIN, 0},
        { (void *) back, 0, ZMQ_POLLIN, 0}
    };
    front.bind (base + argv[1]);
    back.bind (base + argv[2]);
    std::list<std::string> workers;
    while (true)
    {
        zmq::poll (pollers, 2, -1);
        if (pollers[BACK].revents & ZMQ_POLLIN)
            fwd_rep (back, front, workers);
        if (pollers[FRONT].revents & ZMQ_POLLIN)
            fwd_req (front, back, workers);
    }
    return 0;
}

void fwd_req (zmq::socket_t &f, zmq::socket_t &b, std::list<std::string> &workers)
{
    static const char err[] = "no available workers";
    std::stringstream ss;
    int more;
    size_t msize = sizeof (int);
    zmq::message_t cl_id, cl_id1;
    std::string content;
    f.recv (&cl_id);
    do
    {
        zmq::message_t msg;
        f.recv (&msg);
        ss.write ((char *)msg.data(), msg.size());
        f.getsockopt (ZMQ_RCVMORE, &more, &msize);
    }
    while (more);
    if (workers.size())
    {
        content = ss.str();
        std::string worker = workers.front();
        workers.pop_front();
        b.send ((void *)worker.c_str(), worker.size(), ZMQ_SNDMORE);
        b.send ("", 0, ZMQ_SNDMORE);
        b.send (cl_id, ZMQ_SNDMORE);
        b.send ("", 0, ZMQ_SNDMORE);
        b.send ((void *)content.c_str(), content.size());
    }
    else
    {
        cl_id1.copy (&cl_id);
        f.send (cl_id, ZMQ_SNDMORE);
        f.send (err, strlen (err), ZMQ_SNDMORE);
        f.send (cl_id1, ZMQ_SNDMORE);
        f.send ("", 0, ZMQ_SNDMORE);
    }
    return;
}

void fwd_rep (zmq::socket_t &b, zmq::socket_t &f, std::list<std::string> &workers)
{
    std::stringstream ss;
    zmq::message_t id;
    zmq::message_t cl_id, cl_id1;
    char empty[1] = {};
    b.recv (&id);
    b.recv (empty, 1);
    int more;
    size_t msize = sizeof (int);
    b.recv (&cl_id);
    if (strncmp ((char *)cl_id.data(), "__ready", 7) != 0)
    {
        b.recv (empty, 1);
        do
        {
            zmq::message_t msg;
            b.recv (&msg);
            ss.write ((char *)msg.data(), msg.size());
            b.getsockopt (ZMQ_RCVMORE, &more, &msize);
        }
        while (more);
        cl_id1.copy (&cl_id);
        f.send (cl_id, ZMQ_SNDMORE);
        f.send ((void *)ss.str().c_str(), ss.str().size(), ZMQ_SNDMORE);
        f.send (cl_id1, ZMQ_SNDMORE);
        f.send ("", 0, ZMQ_SNDMORE);
    }
    workers.push_back (std::string ((char *)id.data(), id.size()));
    return;
}
