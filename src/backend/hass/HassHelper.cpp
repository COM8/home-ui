#include "HassHelper.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <cstdint>
#include <memory>
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

void set_light_color_temp(uint16_t mireds, const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
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

std::optional<std::string> parse_friendly_name_j(const nlohmann::json& j) {
    try {
        if (!j.contains("attributes")) {
            SPDLOG_ERROR("Error parsing friendly name from '{}' with: 'attributes' not found", j.dump());
            return std::nullopt;
        }
        nlohmann::json jAttr = j["attributes"];

        if (!jAttr.contains("friendly_name")) {
            SPDLOG_ERROR("Error parsing friendly name from '{}' with: 'friendly_name' not found", j.dump());
            return std::nullopt;
        }
        std::string friendlyName;
        jAttr.at("friendly_name").get_to(friendlyName);
        return friendlyName;
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing friendly name from '{}' with: {}", j.dump(), e.what());
    }
    return std::nullopt;
}

std::optional<std::string> parse_friendly_name(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return parse_friendly_name_j(j);
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing json from '{}' with: {}", response, e.what());
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

std::string parse_state_j(const nlohmann::json& j) {
    try {
        if (!j.contains("state")) {
            SPDLOG_ERROR("Error parsing state from '{}' with: 'state' not found", j);
            return "";
        }
        std::string state;
        j["state"].get_to(state);
        return state;
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing state from '{}' with: {}", j, e.what());
    }
    return "";
}

std::string parse_state(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return parse_state_j(j);
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing json from '{}' with: {}", response, e.what());
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

std::shared_ptr<HassLight> get_light_info(const std::string& entity, const std::string& hassIp, const std::string& hassPort, const std::string& bearerToken) {
    SPDLOG_INFO("Requesting light info for '{}'...", entity);
    cpr::Response response = cpr::Get(cpr::Url{"http://" + hassIp + ":" + hassPort + "/api/states/" + entity},
                                      cpr::Bearer{bearerToken},
                                      cpr::Header{{"Content-Type", "application/json"}});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting light info for '{}' failed. Status code: {}\nResponse: {}", entity, response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting light info for '{}' failed. Status code: {}\nError: {}", entity, response.status_code, response.error.message);
        }
        return nullptr;
    }

    SPDLOG_DEBUG("Received from HASS: {}", response.text);

    try {
        nlohmann::json j = nlohmann::json::parse(response.text);
        return HassLight::from_json(j);
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing json from '{}' with: {}", response.text, e.what());
    }
    return nullptr;
}

std::shared_ptr<HassLight> HassLight::from_json(const nlohmann::json& j) {
    std::optional<std::string> friendlyNameOpt = parse_friendly_name_j(j);
    if (!friendlyNameOpt) {
        return nullptr;
    }
    const std::string state = parse_state_j(j);
    if (state == "unavailable") {
        return nullptr;
    }
    bool isOn = state == "on";

    if (!j.contains("attributes")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'attributes' not found", j.dump());
        return nullptr;
    }

    const nlohmann::json& jAttr = j["attributes"];

    if (!jAttr.contains("color_mode")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'color_mode' not found", j.dump());
        return nullptr;
    }
    std::string colorMode;
    jAttr.at("color_mode").get_to(colorMode);
    bool inRbgMode = colorMode == "rgb";

    if (!jAttr.contains("brightness")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'brightness' not found", j.dump());
        return nullptr;
    }
    uint8_t brightness = 0;
    jAttr.at("brightness").get_to(brightness);

    uint16_t colorTemp = 0;
    if (!inRbgMode) {
        if (!jAttr.contains("color_temp")) {
            SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'color_temp' not found", j.dump());
            return nullptr;
        }
        jAttr.at("color_temp").get_to(colorTemp);
    }

    if (!jAttr.contains("min_mireds")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'min_mireds' not found", j.dump());
        return nullptr;
    }
    uint16_t minColorTemp = 0;
    jAttr.at("min_mireds").get_to(minColorTemp);

    if (!jAttr.contains("max_mireds")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'max_mireds' not found", j.dump());
        return nullptr;
    }
    uint16_t maxColorTemp = 0;
    jAttr.at("max_mireds").get_to(maxColorTemp);

    if (!jAttr.contains("hs_color")) {
        SPDLOG_ERROR("Error parsing HassLight from '{}' with: 'hs_color' not found", j.dump());
        return nullptr;
    }
    nlohmann::json::array_t array;
    jAttr.at("hs_color").get_to(array);
    float hue = 0;
    array[0].get_to(hue);
    float saturation = 0;
    array[1].get_to(saturation);

    return std::make_shared<HassLight>(HassLight{*friendlyNameOpt, isOn, inRbgMode, brightness, colorTemp, minColorTemp, maxColorTemp, hue, saturation});
}
}  // namespace backend::hass