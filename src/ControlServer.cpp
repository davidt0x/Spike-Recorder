#include <string>

#include "ControlServer.h"
#include "Log.h"

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

    std::string ControlServer::getMessage()
    {

        zmq::message_t request;

        //  Get the next reequest from the client, do not wait
        auto res = socket.recv(request, zmq::recv_flags::dontwait);
       
        // Did we get a control command
        if (res)
        {
            std::string message = request.to_string();
            BackyardBrains::Log::msg("ControlSever: Message Received %s", message);

            //  Send reply back to client
            zmq::message_t reply(3);
            memcpy((void*)reply.data(), "OK!", 3);
            socket.send(reply);

            return message;
        }

        return "";
 
    }

}