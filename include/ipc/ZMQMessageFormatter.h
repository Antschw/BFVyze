#ifndef ZMQ_MESSAGE_FORMATTER_H
#define ZMQ_MESSAGE_FORMATTER_H

#include <vector>
#include <zmq.hpp>

#include "IPCManager.h"

namespace ipc {

/**
 * @class ZMQMessageFormatter
 * @brief Formats messages for ZeroMQ communication.
 */
class ZMQMessageFormatter {
public:
    /**
     * @brief Formats a BMP buffer into a ZeroMQ message.
     * @param bmpBuffer The buffer containing BMP data.
     * @return A ZeroMQ message containing the BMP data.
     */
    static zmq::message_t formatBMPMessage(const std::vector<BYTE> &bmpBuffer);
};

} // namespace ipc

#endif // ZMQ_MESSAGE_FORMATTER_H
