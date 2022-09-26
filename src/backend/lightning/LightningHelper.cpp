#include "LightningHelper.hpp"
#include "Lightning.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include <cassert>
#include <ctime>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

namespace backend::lightning {
void LightningHelper::start() {
    webSocket.setUrl("wss://live.lightningmaps.org");
    webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) { this->on_message(msg); });
    webSocket.start();
}

void LightningHelper::stop() {
    webSocket.stop();
}

LightningHelper* get_instance() {
    static LightningHelper instance;
    return &instance;
}

void LightningHelper::set_coordinates(double latCenter, double longCenter, double zoomFactor, double latMax, double longMax, double latMin, double longMin) {
    assert(webSocket.getReadyState() == ix::ReadyState::Open);
    nlohmann::json j;
    j["v"] = 24;
    nlohmann::json i;
    i["2"] = 61558531;
    j["i"] = i;
    j["s"] = false;
    j["x"] = 0;
    j["w"] = 0;
    j["tx"] = 0;
    j["tw"] = 0;
    j["a"] = 4;
    j["z"] = 8;
    j["b"] = false;
    j["h"] = "#m=oss;t=3;s=0;o=0;b=;ts=0;z=" + std::to_string(zoomFactor) + ";y=" + std::to_string(latCenter) + ";x=" + std::to_string(longCenter) + ";d=2;dl=2;dc=0;";
    j["l"] = 600;
    j["t"] = static_cast<size_t>(std::time(nullptr));
    j["from_lightningmaps_org"] = true;
    nlohmann::json p;
    p["0"] = latMax;
    p["1"] = longMax;
    p["2"] = latMin;
    p["3"] = longMin;
    j["p"] = p;
    j["r"] = "w";

    SPDLOG_DEBUG("[WEBSOCKET]: Wrote: {}", j.dump());
    webSocket.sendText(j.dump());
}

void LightningHelper::on_message(const ix::WebSocketMessagePtr& msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
        SPDLOG_DEBUG("[WEBSOCKET]: {}", msg->str);
        parse(msg->str);
    } else if (msg->type == ix::WebSocketMessageType::Open) {
        SPDLOG_INFO("[WEBSOCKET]: Connection established.");
        webSocket.sendText(R"({"v":24,"i":{},"s":false,"x":0,"w":0,"tx":0,"tw":1,"a":4,"z":12,"b":true,"h":"#m=oss;t=3;s=0;o=0;b=;ts=0;z=12;y=48.1358;x=11.6228;d=2;dl=2;dc=0;","l":1,"t":1,"from_lightningmaps_org":true,"p":[48.3,12,48,11.2],"r":"A"})");
    } else if (msg->type == ix::WebSocketMessageType::Error) {
        SPDLOG_ERROR("[WEBSOCKET]: Error: {}", msg->errorInfo.reason);
    }
}

void LightningHelper::parse(const std::string& s) {
    try {
        nlohmann::json j = nlohmann::json::parse(s);
        if (j.contains("strokes")) {
            nlohmann::json::array_t array;
            j.at("strokes").get_to(array);
            std::vector<Lightning> lightnings;
            for (const nlohmann::json& jLighting : array) {
                std::optional<Lightning> lighting = Lightning::from_json(jLighting);
                if (lighting) {
                    lightnings.emplace_back(*lighting);
                }
            }
            SPDLOG_INFO("Received {} new lightnings.", lightnings.size());
        }
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing weather from '{}' with: {}", s, e.what());
    }
}

}  // namespace backend::lightning
