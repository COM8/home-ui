#include "Departure.hpp"
#include "logger/Logger.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <bits/types/time_t.h>
#include <spdlog/spdlog.h>

namespace backend::mvg {
std::shared_ptr<Departure> Departure::from_json(const nlohmann::json& j) {
    if (!j.contains("departureTime")) {
        SPDLOG_ERROR("Failed to parse departure. 'departureTime' filed missing.");
        return nullptr;
    }
    int64_t departureTime = 0;  // In ms Unix time
    j.at("departureTime").get_to(departureTime);
    std::chrono::system_clock::time_point departureTimeTp = std::chrono::system_clock::from_time_t(static_cast<time_t>(departureTime / 1000));

    if (!j.contains("product")) {
        SPDLOG_ERROR("Failed to parse departure. 'product' filed missing.");
        return nullptr;
    }
    std::string product;
    j.at("product").get_to(product);
    ProductType pType = ProductType::UNKNOWN;
    if (product == "UBAHN") {
        pType = ProductType::U_BAHN;
    } else if (product == "SBAHN") {
        pType = ProductType::S_BAHN;
    } else if (product == "BUS") {
        pType = ProductType::BUS;
    } else if (product == "TRAM") {
        pType = ProductType::TRAM;
    } else {
        SPDLOG_WARN("Unknown MVG product type '{}'.", product);
    }

    if (!j.contains("label")) {
        SPDLOG_ERROR("Failed to parse departure. 'label' filed missing.");
        return nullptr;
    }
    std::string label;
    j.at("label").get_to(label);

    if (!j.contains("destination")) {
        SPDLOG_ERROR("Failed to parse departure. 'destination' filed missing.");
        return nullptr;
    }
    std::string destination;
    j.at("destination").get_to(destination);

    // Delay is optional:
    int delay = 0;
    if (j.contains("delay")) {
        j.at("delay").get_to(delay);
    }

    if (!j.contains("cancelled")) {
        SPDLOG_ERROR("Failed to parse departure. 'cancelled' filed missing.");
        return nullptr;
    }
    bool cancelled = false;
    j.at("cancelled").get_to(cancelled);

    if (!j.contains("lineBackgroundColor")) {
        SPDLOG_ERROR("Failed to parse departure. 'lineBackgroundColor' filed missing.");
        return nullptr;
    }
    std::string lineBackgroundColor;
    j.at("lineBackgroundColor").get_to(lineBackgroundColor);

    if (!j.contains("platform")) {
        SPDLOG_ERROR("Failed to parse departure. 'platform' filed missing.");
        return nullptr;
    }
    std::string platform;
    j.at("platform").get_to(platform);

    // Is of type array:
    // if (!j.contains("infoMessages")) {
    //     SPDLOG_ERROR("Failed to parse departure. 'infoMessages' filed missing.");
    //     return nullptr;
    // }
    // std::string infoMessages;
    // j.at("infoMessages").get_to(infoMessages);

    return std::make_shared<Departure>(Departure{departureTimeTp,
                                                 pType,
                                                 label,
                                                 destination,
                                                 delay,
                                                 cancelled,
                                                 lineBackgroundColor,
                                                 platform,
                                                 ""});
}
}  // namespace backend::mvg
