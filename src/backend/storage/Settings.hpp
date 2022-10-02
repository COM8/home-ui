#pragma once

#include "backend/storage/Serializer.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace backend::storage {
struct SettingsData {
    /**
     * The location id, where the MVG widget should fetch departures for.
     * Default value: "de:09162:530" (München Alte Heide)
     **/
    std::string mvgLocation{"de:09162:530"};
    bool mvgBusEnabled = true;
    bool mvgTramEnabled = true;
    bool mvgSBahnEnabled = true;
    bool mvgUBahnEnabled = true;
    std::string mvgDestRegex{"Garching, Forschungszentrum"};
    bool mvgDestRegexEnabled = true;

    /**
     * The open weather map location for the weather forecast. 
     **/
    std::string weatherLat{"48.137154"};
    std::string weatherLong{"11.576124"};
    std::string openWeatherApiKey{"https://openweathermap.org/price"};

    /**
     * A list of all device IPs there status should be displayed.
     **/
    std::vector<std::string> devices;

    /**
     * The Home Assistent (hass) configuration and devices.
     **/
    std::string hassIp{"IP of your hass installation"};
    std::string hassPort{"8123"};
    std::string hassBearerToken{"Your hass bearer token"};
    std::vector<std::string> hassLights;

    /**
     * Lightning map configuration.
     **/
    double lightningMapCenterLat{48.137154};
    double lightningMapCenterLong{11.576124};
    double lightningMapHomeLat{48.137154};
    double lightningMapHomeLong{11.576124};
    double lightningMapZoomLevel{11};

} __attribute__((aligned(128)));

class Settings {
 public:
    explicit Settings(const std::filesystem::path& configFilePath);

    SettingsData data{};
    void write_settings();

 private:
    storage::Serializer fileHandle;
};

Settings* get_settings_instance();
}  // namespace backend::storage
