#include "backend/storage/Serializer.hpp"
#include "backend/storage/Settings.hpp"

#include <chrono>
#include <cstddef>
#include <functional>
namespace backend::storage {
void to_json(nlohmann::json& j, const SettingsData& d) {
    j = nlohmann::json{
        {"mvgLocation", d.mvgLocation},
        {"mvgBusEnabled", d.mvgBusEnabled},
        {"mvgSBahnEnabled", d.mvgSBahnEnabled},
        {"mvgUBahnEnabled", d.mvgUBahnEnabled},
        {"mvgTramEnabled", d.mvgTramEnabled},
        {"mvgDestRegex", d.mvgDestRegex},
        {"mvgDestRegexEnabled", d.mvgDestRegexEnabled},

        {"weatherLat", d.weatherLat},
        {"weatherLong", d.weatherLong},
        {"openWeatherApiKey", d.openWeatherApiKey},

        {"devices", d.devices},

        {"hassIp", d.hassIp},
        {"hassPort", d.hassPort},
        {"hassBearerToken", d.hassBearerToken},
        {"hassLights", d.hassLights},

        {"lightningMapCenterLat", d.lightningMapCenterLat},
        {"lightningMapCenterLong", d.lightningMapCenterLong},
        {"lightningMapHomeLat", d.lightningMapHomeLat},
        {"lightningMapHomeLong", d.lightningMapHomeLong},
        {"lightningMapZoomLevel", d.lightningMapZoomLevel}};
}

void from_json(const nlohmann::json& j, SettingsData& d) {
    j.at("mvgLocation").get_to(d.mvgLocation);
    j.at("mvgBusEnabled").get_to(d.mvgBusEnabled);
    j.at("mvgSBahnEnabled").get_to(d.mvgSBahnEnabled);
    j.at("mvgUBahnEnabled").get_to(d.mvgUBahnEnabled);
    j.at("mvgTramEnabled").get_to(d.mvgTramEnabled);
    j.at("mvgDestRegex").get_to(d.mvgDestRegex);
    j.at("mvgDestRegexEnabled").get_to(d.mvgDestRegexEnabled);

    j.at("weatherLat").get_to(d.weatherLat);
    j.at("weatherLong").get_to(d.weatherLong);
    j.at("openWeatherApiKey").get_to(d.openWeatherApiKey);

    j.at("devices").get_to(d.devices);

    j.at("hassIp").get_to(d.hassIp);
    j.at("hassPort").get_to(d.hassPort);
    j.at("hassBearerToken").get_to(d.hassBearerToken);
    j.at("hassLights").get_to(d.hassLights);

    j.at("lightningMapCenterLat").get_to(d.lightningMapCenterLat);
    j.at("lightningMapCenterLong").get_to(d.lightningMapCenterLong);
    j.at("lightningMapHomeLat").get_to(d.lightningMapHomeLat);
    j.at("lightningMapHomeLong").get_to(d.lightningMapHomeLong);
    j.at("lightningMapZoomLevel").get_to(d.lightningMapZoomLevel);
}
}  // namespace backend::storage

namespace nlohmann {
void adl_serializer<std::chrono::system_clock::time_point>::to_json(json& j, const std::chrono::system_clock::time_point& tp) {
    j = std::chrono::system_clock::to_time_t(tp);
}

void adl_serializer<std::chrono::system_clock::time_point>::from_json(const json& j, std::chrono::system_clock::time_point& tp) {
    std::time_t t = 0;
    j.get_to(t);
    tp = std::chrono::system_clock::from_time_t(t);
}
}  // namespace nlohmann
