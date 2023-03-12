#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::db {
struct Stop {
    std::string name;
    bool canceled;
    bool showVia;
} __attribute__((aligned(64)));

struct Departure {
    std::chrono::system_clock::time_point depTime;
    std::chrono::system_clock::time_point depTimeScheduled;
    std::string platform;
    std::string platformScheduled;
    int delay;
    bool canceled;
    std::string productClass;
    std::string trainName;

    std::string destination;
    std::string destinationScheduled;

    std::string curStopPlace;
    std::vector<Stop> prevStops;
    std::vector<Stop> nextStops;

    std::vector<std::string> infoMessages;

    static std::shared_ptr<Departure> from_json(const nlohmann::json& j);
} __attribute__((aligned(128)));
}  // namespace backend::db