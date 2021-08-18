#include "OpenWeatherHelper.hpp"
#include "logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <cpr/cpr.h>

namespace backend::weather {
cpr::Url build_url(const std::string& lat, const std::string& lon, const std::string& apiKey) {
    return cpr::Url("https://api.openweathermap.org/data/2.5/onecall?lat=" + lat + "&lon=" + lon + "&exclude=minutely&units=metric&appid=" + apiKey);
}

std::shared_ptr<Forecast> parse_response(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        return std::make_shared<Forecast>(Forecast::from_json(j));
    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing weather from '{}' with: {}", response, e.what());
    }
    return nullptr;
}

std::shared_ptr<Forecast> request_weather(const std::string& lat, const std::string& lon, const std::string& apiKey) {
    cpr::Session session;
    session.SetUrl(build_url(lat, lon, apiKey));

    SPDLOG_DEBUG("Requesting weather for lat '{}', lon '{}'...", lat, lon);
    cpr::Response response = session.Get();
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting weather failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting weather failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return nullptr;
    }
    SPDLOG_DEBUG("Weather requested successfully. Parsing...");
    return parse_response(response.text);
}
}  // namespace backend::weather
