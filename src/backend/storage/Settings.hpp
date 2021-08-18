#pragma once

#include "backend/storage/Serializer.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>

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

    /**
     * The open weather map location for the weather forecast. 
     **/
    std::string weatherLat{"48.137154"};
    std::string weatherLong{"11.576124"};
    std::string openWeatherApiKey{"https://openweathermap.org/price"};

} __attribute__((aligned(128)));

class Settings {
 public:
    explicit Settings(const std::filesystem::path& configFilePath);

    SettingsData data{};

 private:
    storage::Serializer fileHandle;

    void write_settings();
};

Settings* get_settings_instance();
}  // namespace backend::storage
