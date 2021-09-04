#include "HassHelper.hpp"
#include "logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <cpr/api.h>

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
    return nullptr;
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

    std::optional<std::string> friendlyName = parse_friendly_name(response.text);
    if (friendlyName) {
        SPDLOG_INFO("Friendly name for '{}' found: {}", entity);
        return *friendlyName;
    }
    return entity;
}
}  // namespace backend::hass