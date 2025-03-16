#include "ipc/ZMQClient.h"
#include <spdlog/spdlog.h>

namespace ipc {

ZMQClient::ZMQClient() : context_(1), socket_(context_, zmq::socket_type::push) {}

ZMQClient::~ZMQClient() {
    socket_.close();
    context_.close();
}

bool ZMQClient::connect(const std::string &endpoint) {
    try {
        socket_.connect(endpoint);
        spdlog::info("ZMQClient connected to endpoint: {}", endpoint);
        return true;
    } catch (const zmq::error_t &e) {
        spdlog::error("ZMQClient connection error: {}", e.what());
        return false;
    }
}

bool ZMQClient::sendMessage(zmq::message_t message) {
    try {
        socket_.send(std::move(message), zmq::send_flags::none);
        spdlog::info("ZMQClient sent message ({} bytes)", message.size());
        return true;
    } catch (const zmq::error_t &e) {
        spdlog::error("ZMQClient send error: {}", e.what());
        return false;
    }
}

} // namespace ipc
