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
    SPDLOG_INFO("Lightning websocket started.");
}

void LightningHelper::stop() {
    webSocket.stop();
    SPDLOG_INFO("Lightning websocket stopped.");
}

LightningHelper* get_instance() {
    static LightningHelper instance;
    return &instance;
}

void LightningHelper::set_coordinates(double latCenter, double longCenter, double zoomFactor, double latMax, double longMax, double latMin, double longMin) {
    if (this->latCenter != latCenter) {
        coordinatesChanged = true;
    }
    this->latCenter = latCenter;

    if (this->longCenter != longCenter) {
        coordinatesChanged = true;
    }
    this->longCenter = longCenter;

    if (this->zoomFactor != zoomFactor) {
        coordinatesChanged = true;
    }
    this->zoomFactor = zoomFactor;

    if (this->latMax != latMax) {
        coordinatesChanged = true;
    }
    this->latMax = latMax;

    if (this->longMax != longMax) {
        longMax = true;
    }
    this->longMax = longMax;

    if (this->latMin != latMin) {
        coordinatesChanged = true;
    }
    this->latMin = latMin;

    if (this->longMin != longMin) {
        coordinatesChanged = true;
    }
    this->longMin = longMin;
}

void LightningHelper::set_coordinates() {
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
        if (coordinatesChanged) {
            coordinatesChanged = false;
            set_coordinates();
        }
    } else if (msg->type == ix::WebSocketMessageType::Open) {
        SPDLOG_INFO("[WEBSOCKET]: Connection established.");
        set_coordinates();
    } else if (msg->type == ix::WebSocketMessageType::Error) {
        SPDLOG_ERROR("[WEBSOCKET]: Error: {}", msg->errorInfo.reason);
    }
}

void LightningHelper::parse(const std::string& s) {
    try {
        const nlohmann::json j = nlohmann::json::parse(s);
        if (j.contains("strokes")) {
            nlohmann::json::array_t array;
            j.at("strokes").get_to(array);
            std::vector<Lightning> lightnings;
            for (const nlohmann::json& jLighting : array) {
                std::optional<Lightning> lighting = Lightning::from_json(jLighting);
                if (lighting && lighting->lat >= latMin && lighting->lat <= latMax && lighting->lon >= longMin && lighting->lon <= longMax) {
                    lightnings.emplace_back(*lighting);
                }
            }

            if (lightnings.empty()) {
                return;
            }
            SPDLOG_INFO("Received {} new lightnings.", lightnings.size());

            // Invoke the event handler:
            if (newLightningsEventHandler) {
                newLightningsEventHandler(lightnings);
            }
        }
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing weather from '{}' with: {}", s, e.what());
    }
}

}  // namespace backend::lightning
