#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include <string>
#include <windows.h>
#include "ipc/ZMQClient.h"

namespace ipc {

class IPCManager {
public:
    /**
     * @brief Constructor with the destination ZeroMQ address.
     */
    explicit IPCManager(std::string endpoint);

    /**
     * @brief Captures the image, converts it to black and white, and sends it via ZeroMQ.
     * @param hBitmap Handle of the captured bitmap.
     * @param width Width of the image.
     * @param height Height of the image.
     * @return True if the sending was successful, false otherwise.
     */
    bool sendBlackAndWhiteImage(HBITMAP hBitmap, int width, int height);

private:
    ZMQClient   zmqClient_;
    std::string endpoint_;
};

} // namespace ipc

#endif // IPC_MANAGER_H
