#pragma once

#include <string>

namespace backend::hass {
void toggle_light(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
std::string get_friendly_name(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken);
}  // namespace backend::hass