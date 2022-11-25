#pragma once

#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace backend::hass {
struct HassLight {
    std::string friendlyName;
    bool isOn;
    bool inRbgMode;
    uint8_t brightness;
    uint16_t colorTemp;
    uint16_t minColorTemp;
    uint16_t maxColorTemp;
    double hue;
    double saturation;

    static std::shared_ptr<HassLight> from_json(const nlohmann::json& j);
} __attribute__((aligned(64)));

void toggle_light(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_brightness(uint8_t brightness, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_color_temp(uint16_t mireds, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_color(double hue, double saturation, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
std::string get_friendly_name(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
std::shared_ptr<HassLight> get_light_info(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
bool is_light_on(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
}  // namespace backend::hass