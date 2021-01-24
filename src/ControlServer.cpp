#include <string>

#include "ControlServer.h"
#include "MainView.h"
#include "Log.h"

#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

namespace BackyardBrains {
    

    ControlServer::ControlServer() {

    }

    void ControlServer::start() {
        
        //  Prepare our context and socket
        context = zmq::context_t(1);
        socket = zmq::socket_t(context, ZMQ_REP);
        socket.bind("tcp://127.0.0.1:5555");

        BackyardBrains::Log::msg("ControlSever: Started (localhost:5555)");

    }
    
    void ControlServer::stop() {

        // Shut if all down
        socket.close();
        context.close();
        BackyardBrains::Log::msg("ControlSever: Closed");

    }

    void ControlServer::reply_ok()
    {
        //  Send success reply back to client
        auto reply_ok = R"(
                {
                    "type": "REPLY_OK"
                }
                )"_json;
        auto length = std::snprintf(nullptr, 0, "%s", reply_ok.dump().c_str()) + 1;
        // +1 to account for null terminating character.

        zmq::message_t message(length);
        std::snprintf(static_cast<char*>(message.data()), length, "%s", reply_ok.dump().c_str());
        socket.send(message, zmq::send_flags::none);
    }

    void ControlServer::reply_error(std::string error)
    {
        //  Send success reply back to client
        auto reply_ok = R"(
                {
                    "type": "REPLY_ERROR"
                }
                )"_json;
        auto length = std::snprintf(nullptr, 0, "%s", reply_ok.dump().c_str()) + 1;
        // +1 to account for null terminating character.

        zmq::message_t message(length);
        std::snprintf(static_cast<char*>(message.data()), length, "%s", reply_ok.dump().c_str());
        socket.send(message, zmq::send_flags::none);
    }

    void ControlServer::processMessage(BackyardBrains::MainView * main_view)
    {

        zmq::message_t request;

        //  Get the next reequest from the client, do not wait
        auto res = socket.recv(request, zmq::recv_flags::dontwait);
       
        // Did we get a control command
        if (res)
        {
            std::string message = request.to_string();
            BackyardBrains::Log::msg("ControlSever: Message Received %s", message.c_str());

            // Parse the json message
            try {
                auto j = json::parse(message);
                auto type = j["type"].get<std::string>();
                auto args = j["args"];

                // Figure out what to do
                if (type == "SHUTDOWN")
                {
                    main_view->shutdown();
                }
                else if (type == "PUSH_EVENT_MARKER") {
                    main_view->addMarker(args["name"].get<std::string>());
                }
                else if (type == "START_RECORD") {
                    if(args.contains("filename") && args["filename"].get<std::string>() != "")
                        main_view->startRecord(args["filename"].get<std::string>());
                    else
                        main_view->startRecord();
                }
                else if (type == "STOP_RECORD") {
                    main_view->stopRecord();
                }
                else {
                    Log::error("ControlServer: Inavlid Command Message Type: %s", type);
                    reply_error("ControlServer: Inavlid Command Message Type");
                }
              

                // Seems to be ok, reply ok              
                reply_ok();
              
            }
            catch (json::exception& e)
            {
                // output exception information
                Log::error("ControlServer: Message JSON Parse Error\n\tmessage: %s", e.what());
                
                // Error, reply with error to client side
                reply_error(std::string(e.what()));

            }
 
        }

    }

}