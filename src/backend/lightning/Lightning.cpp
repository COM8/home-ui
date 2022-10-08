#include "Lightning.hpp"
#include "logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
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
    std::chrono::system_clock::time_point timeTp = std::chrono::system_clock::from_time_t(static_cast<time_t>(time / 1000));
    timeTp += std::chrono::milliseconds(time % 1000);

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

constexpr double PI = 3.14159265358979323846;
constexpr double RADIO_TERRESTRE = 6372797.56085;
constexpr double GRADOS_RADIANES = PI / 180;

double Lightning::distance(double lat, double lon) const {
    // Source: https://stackoverflow.com/a/63767823

    double lat1Tmp = this->lat * GRADOS_RADIANES;
    double long1Tmp = this->lon * GRADOS_RADIANES;
    double lat2Tmp = lat * GRADOS_RADIANES;
    double long2Tmp = lon * GRADOS_RADIANES;

    double haversine = (std::pow(std::sin((1.0 / 2) * (lat2Tmp - lat1Tmp)), 2)) + ((std::cos(lat1Tmp)) * (std::cos(lat2Tmp)) * (std::pow(std::sin((1.0 / 2) * (long2Tmp - long1Tmp)), 2)));
    double tmp = 2 * std::asin(std::min(1.0, std::sqrt(haversine)));
    return RADIO_TERRESTRE * tmp;
}
}  // namespace backend::lightning