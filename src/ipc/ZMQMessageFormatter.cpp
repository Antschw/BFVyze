#include "ipc/ZMQMessageFormatter.h"
#include <cstring>

namespace ipc {

    zmq::message_t ZMQMessageFormatter::formatBMPMessage(const std::vector<BYTE>& bmpBuffer) {
        zmq::message_t message(bmpBuffer.size());
        std::memcpy(message.data(), bmpBuffer.data(), bmpBuffer.size());
        return message;
    }

} // namespace ipc
