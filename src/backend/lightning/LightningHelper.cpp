#include "LightningHelper.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cassert>
#include <memory>
#include <thread>
#include <websocketpp/close.hpp>
#include <websocketpp/error.hpp>

namespace backend::lightning {
void LightningHelper::start() {
    startStopMutex.lock();
    if (shouldRun) {
        SPDLOG_WARN("No need to start lightning helper. Already running.");
        startStopMutex.unlock();
        return;
    }
    assert(!thread);
    shouldRun = true;
    thread = std::make_unique<std::thread>(&LightningHelper::thread_loop, this);
    startStopMutex.unlock();
}

void LightningHelper::stop() {
    startStopMutex.lock();
    if (!shouldRun) {
        SPDLOG_WARN("No need to stop lightning helper. Already running.");
        startStopMutex.unlock();
        return;
    }

    shouldRun = false;
    assert(thread);
    thread->join();
    thread = nullptr;
    startStopMutex.unlock();
}

LightningHelper* get_instance() {
    static LightningHelper instance;
    return &instance;
}

void LightningHelper::thread_loop() {
    client.init_asio();
    client.set_access_channels(websocketpp::log::alevel::all);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    // client.set_message_handler(websocketpp::lib::bind(&on_message, &client, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    // NOLINTNEXTLINE
    client.set_message_handler(websocketpp::lib::bind([this](wsClient* client, websocketpp::connection_hdl hdl, messagePtr msg) { this->on_message(client, hdl, msg); }, &client, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    // client.set_message_handler([](wsClient* /*client*/, websocketpp::connection_hdl /*hdl*/, messagePtr /*msg*/) {});
    websocketpp::lib::error_code ec;
    connection = client.get_connection("live.lightningmaps.org", ec);
    if (ec) {
        SPDLOG_ERROR("Websocket could not create connection: {}", ec.message());
        return;
    }

    while (shouldRun) {
        try {
            client.connect(connection);
            while (shouldRun) {
                client.run_one();
            }
        } catch (const websocketpp::exception& e) {
            SPDLOG_ERROR("Websocket failed with: {} - {}", e.code().message(), e.what());
        }
    }
    // client.close(connection, ); // TODO: Close properly
}

void LightningHelper::on_message(wsClient* /*client*/, websocketpp::connection_hdl hdl, messagePtr msg) {
    SPDLOG_DEBUG("[WEBSOCKET][{}]: ", hdl.lock().get(), msg->get_payload());
}

}  // namespace backend::lightning
