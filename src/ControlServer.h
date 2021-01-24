#pragma once

#include "MainView.h"

#include <zmq.hpp>


namespace BackyardBrains {
    
    class MainView;

    class ControlServer {

    public:
        ControlServer();
        void start();
        void stop();
        void processMessage(MainView * main_view);
        void reply_ok();
        void reply_error(std::string error);
       
    protected:
        zmq::socket_t socket;
        zmq::context_t context;

    };



} // namespace BackyardBrains
