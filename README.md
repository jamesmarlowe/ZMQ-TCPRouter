# ZMQ-TCPRouter
Route traffic to ZMQ_REP sockets from raw TCP with this router

Requisites
----------
* CMake
* The ZMQ libraries (ubuntu/debian package libzmq3-dev)
* A modern C++ compiler supporting C++11 (clang++ 3.x, g++ 4.7+)

Building
--------
```
cd ZMQ-TCPRouter
mkdir build
cd build
cmake ..
make
```

Running
-------
```
./tcprouter 5555 5556
```

Trying
------
a simple example can be found in the testclient and testworker builds
```
./testworker 5556
./testclient 5555
```
Which should send data through the router from client to worker and back
