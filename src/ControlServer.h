#pragma once

#include <zmq.hpp>

namespace BackyardBrains {
    
    class ControlServer {

    public:
        ControlServer();
        void start();
        void stop();
        std::string getMessage();
       
    protected:
        zmq::socket_t socket;
        zmq::context_t context;

    };



} // namespace BackyardBrains
