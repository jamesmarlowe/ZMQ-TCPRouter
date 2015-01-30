cmake_minimum_required (VERSION 2.8)
project (tcprouter)

add_definitions ("-Wno-deprecated-declarations")

set (CMAKE_MODULE_PATH $ {CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#enable c++ 11
SET (CMAKE_CXX_FLAGS "-std=c++11 -O3 ${CMAKE_CXX_FLAGS}")

#zeromqness
find_package(ZMQ REQUIRED)
include_directories(${ZMQ_INCLUDE_DIRS})

include_directories ($ {CMAKE_CURRENT_BINARY_DIR})
include_directories ($ {CMAKE_SOURCE_DIR})

set(tcprouter_sources
    TCPRouter.cpp
)

set(test_worker_sources
    testworker.cpp
)

set(test_client_sources
    testclient.cpp
)

set(fcgi_test_client_sources
    fcgitestclient.cpp
)

add_subdirectory(zmq)

add_executable (tcprouter ${tcprouter_sources})
add_executable (testworker ${test_worker_sources})
add_executable (testclient ${test_client_sources})
add_executable (fcgitestclient ${fcgi_test_client_sources})

target_link_libraries (tcprouter zmq)
target_link_libraries (testworker zmqcpp)
target_link_libraries (testclient zmqcpp)
target_link_libraries (fcgitestclient zmqcpp fcgi++ fcgi)
