#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <ixwebsocket/IXWebSocket.h>

namespace backend::lightning {
class LightningHelper {
 private:
    ix::WebSocket webSocket;

 public:
    void start();
    void stop();
    void set_coordinates(double latCenter, double longCenter, double zoomFactor, double latMax, double longMax, double latMin, double longMin);

 private:
    void on_message(const ix::WebSocketMessagePtr& msg);
    void parse(const std::string& s);
};

LightningHelper* get_instance();
}  // namespace backend::lightning