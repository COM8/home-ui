#include "HassHelper.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <cpr/api.h>
#include <spdlog/spdlog.h>

namespace backend::hass {
void toggle_light(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Toggeling light '{}'...", entity);
    const nlohmann::json bodyJ{{"entity_id", entity}};
    const std::string bodyStr = bodyJ.dump();
    cpr::Response response = cpr::Post(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/services/light/toggle"},
                                       cpr::Bearer{bearerToken},
                                       cpr::Header{{"Content-Type", "application/json"}},
                                       cpr::Body{bodyStr});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Toggeling light '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Toggeling light '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return;
    }
    SPDLOG_INFO("Light '{}' toggled.", entity);
}

void set_light_brightness(uint8_t brightness, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Setting brightness for light '{}' to '{}'...", entity, brightness);
    const nlohmann::json bodyJ{{"entity_id", entity}, {"brightness", brightness}};
    const std::string bodyStr = bodyJ.dump();
    cpr::Response response = cpr::Post(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/services/light/turn_on"},
                                       cpr::Bearer{bearerToken},
                                       cpr::Header{{"Content-Type", "application/json"}},
                                       cpr::Body{bodyStr});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Setting brightness for light '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Setting brightness for light '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return;
    }
    SPDLOG_INFO("Brightness for light '{}' set.", entity);
}

void set_light_color_temp(uint8_t mireds, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Setting color temperature for light '{}' to '{}'...", entity, mireds);
    const nlohmann::json bodyJ{{"entity_id", entity}, {"color_temp", mireds}};
    const std::string bodyStr = bodyJ.dump();
    cpr::Response response = cpr::Post(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/services/light/turn_on"},
                                       cpr::Bearer{bearerToken},
                                       cpr::Header{{"Content-Type", "application/json"}},
                                       cpr::Body{bodyStr});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Setting color temperature for light '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Setting color temperature for light '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return;
    }
    SPDLOG_INFO("Color temperature for light '{}' set.", entity);
}

void set_light_color(double hue, double saturation, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    assert(hue >= 0);
    assert(hue <= 360);
    assert(saturation >= 0);
    assert(saturation <= 100);

    SPDLOG_INFO("Setting color for light '{}' to '{}', '{}'...", entity, hue, saturation);
    nlohmann::json::array_t hsArr{hue, saturation};
    const nlohmann::json bodyJ{{"entity_id", entity}, {"hs_color", hsArr}};
    const std::string bodyStr = bodyJ.dump();
    cpr::Response response = cpr::Post(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/services/light/turn_on"},
                                       cpr::Bearer{bearerToken},
                                       cpr::Header{{"Content-Type", "application/json"}},
                                       cpr::Body{bodyStr});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Setting color for light '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Setting color for light '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return;
    }
    SPDLOG_INFO("Color for light '{}' set.", entity);
}

std::optional<std::string> parse_friendly_name(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (!j.contains("attributes")) {
            SPDLOG_ERROR("Error parsing friendly name from '{}' with: 'attributes' not found", response);
            return std::nullopt;
        }

        if (!j["attributes"].contains("friendly_name")) {
            SPDLOG_ERROR("Error parsing friendly name from '{}' with: 'friendly_name' not found", response);
            return std::nullopt;
        }
        std::string friendlyName;
        j["attributes"].at("friendly_name").get_to(friendlyName);
        return friendlyName;
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing friendly name from '{}' with: {}", response, e.what());
    }
    return std::nullopt;
}

std::string get_friendly_name(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Requesting friendly name for '{}'...", entity);
    cpr::Response response = cpr::Get(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/states/" + entity},
                                      cpr::Bearer{bearerToken},
                                      cpr::Header{{"Content-Type", "application/json"}});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting friendly name for '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting friendly name for '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return entity;
    }

    SPDLOG_DEBUG("Received from HASS: {}", response.text);
    std::optional<std::string> friendlyName = parse_friendly_name(response.text);
    if (friendlyName) {
        SPDLOG_INFO("Friendly name for '{}' found: {}", entity, *friendlyName);
        return *friendlyName;
    }
    return entity;
}

std::string parse_state(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        if (!j.contains("state")) {
            SPDLOG_ERROR("Error parsing state from '{}' with: 'state' not found", response);
            return "";
        }
        std::string state;
        j["state"].get_to(state);
        return state;
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing state from '{}' with: {}", response, e.what());
    }
    return "";
}

bool is_light_on(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Requesting friendly name for '{}'...", entity);
    cpr::Response response = cpr::Get(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/states/" + entity},
                                      cpr::Bearer{bearerToken},
                                      cpr::Header{{"Content-Type", "application/json"}});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting friendly name for '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting friendly name for '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return false;
    }

    SPDLOG_DEBUG("Received from HASS: {}", response.text);
    return parse_state(response.text) == "on";
}
}  // namespace backend::hass