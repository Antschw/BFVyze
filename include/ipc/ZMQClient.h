#ifndef ZMQ_CLIENT_H
#define ZMQ_CLIENT_H

#include <zmq.hpp>
#include <string>

namespace ipc {

    /**
     * @brief Class representing a ZeroMQ client for sending messages.
     */
    class ZMQClient {
    public:
        /**
         * @brief Constructor for ZMQClient.
         */
        ZMQClient();

        /**
         * @brief Destructor for ZMQClient.
         */
        ~ZMQClient();

        /**
         * @brief Connects the client to a specified ZeroMQ endpoint.
         * @param endpoint The ZeroMQ endpoint to connect to.
         * @return True if the connection was successful, false otherwise.
         */
        bool connect(const std::string& endpoint);

        /**
         * @brief Sends a message using ZeroMQ.
         * @param message The message to be sent.
         * @return True if the message was sent successfully, false otherwise.
         */
        bool sendMessage(zmq::message_t message);

    private:
        zmq::context_t context_; ///< ZeroMQ context for managing sockets.
        zmq::socket_t socket_;   ///< ZeroMQ socket for communication.
    };

}

#endif // ZMQ_CLIENT_H
