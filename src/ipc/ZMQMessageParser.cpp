#include "ipc/ZMQMessageParser.h"
#include <string>

namespace ipc {

    std::string ZMQMessageParser::parseResponse(const zmq::message_t& message) {
        return {static_cast<const char*>(message.data()), message.size()};
    }

} // namespace ipc
