#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace backend::mvg {
enum ProductType {
    UNKNOWN,
    U_BAHN,
    BUS,
    S_BAHN,
    TRAM
};

struct Departure {
    std::chrono::system_clock::time_point time;
    ProductType type;
    std::string label;
    std::string destination;
    int delay;
    bool canceled;
    std::string lineBackgroundColor;
    std::string platform;
    std::string infoMessage;

    static std::shared_ptr<Departure> from_json(const nlohmann::json& j);
} __attribute__((aligned(128)));
}  // namespace backend::mvg