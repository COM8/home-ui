#pragma once

#include <cstdint>
#include <string>

namespace backend::hass {
void toggle_light(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_bightness(uint8_t brightness, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_color_temp(uint8_t mireds, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
void set_light_color(double hue, double saturation, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
std::string get_friendly_name(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
}  // namespace backend::hass