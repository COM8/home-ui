#include "Lightning.hpp"
#include "logger/Logger.hpp"
#include <memory>
#include <optional>

namespace backend::lightning {
std::optional<Lightning> Lightning::from_json(const nlohmann::json& j) {
    if (!j.contains("time")) {
        SPDLOG_ERROR("Failed to parse lightning. 'time' filed missing.");
        return std::nullopt;
    }
    int64_t time = 0;  // In ms Unix time
    j.at("time").get_to(time);
    std::chrono::system_clock::time_point timeTp = std::chrono::system_clock::from_time_t(static_cast<time_t>(time));

    if (!j.contains("lat")) {
        SPDLOG_ERROR("Failed to parse lightning. 'lat' filed missing.");
        return std::nullopt;
    }
    double lat = 0;
    j.at("lat").get_to(lat);

    if (!j.contains("lon")) {
        SPDLOG_ERROR("Failed to parse lightning. 'lon' filed missing.");
        return std::nullopt;
    }
    double lon = 0;
    j.at("lon").get_to(lon);

    return std::make_optional<Lightning>(Lightning{lat, lon, timeTp});
}
}  // namespace backend::lightning