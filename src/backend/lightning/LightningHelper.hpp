#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace backend::lightning {
class LightningHelper {
 private:
    using wsClient = websocketpp::client<websocketpp::config::asio_client>;
    using messagePtr = websocketpp::config::asio_client::message_type::ptr;

    wsClient client{};
    wsClient::connection_ptr connection{nullptr};
    std::unique_ptr<std::thread> thread{nullptr};
    bool shouldRun{false};
    std::mutex startStopMutex{};

 public:
    void start();
    void stop();
    void set_coordinates();

 private:
    void thread_loop();
    void on_message(wsClient* client, websocketpp::connection_hdl hdl, messagePtr msg);
};

LightningHelper* get_instance();
}  // namespace backend::lightning