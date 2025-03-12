#ifndef ZMQ_MESSAGE_PARSER_H
#define ZMQ_MESSAGE_PARSER_H

#include <string>
#include <zmq.hpp>

namespace ipc {

    /**
     * @class ZMQMessageParser
     * @brief Parses responses from ZeroMQ messages.
     */
    class ZMQMessageParser {
    public:
        /**
         * @brief Parses a ZeroMQ message to extract the response as a string.
         * @param message The ZeroMQ message to parse.
         * @return The extracted response as a string.
         */
        static std::string parseResponse(const zmq::message_t &message);
    };

}

#endif // ZMQ_MESSAGE_PARSER_H
