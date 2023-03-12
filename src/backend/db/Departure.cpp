#include "Departure.hpp"
#include "backend/date/date.hpp"
#include "logger/Logger.hpp"
#include "nlohmann/json.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <bits/types/time_t.h>
#include <spdlog/spdlog.h>

namespace backend::db {
std::vector<std::string> parse_info_messages(const nlohmann::json& j) {
    if (!j.contains("messages")) {
        return {};
    }
    nlohmann::json messagesObj;
    j.at("messages").get_to(messagesObj);

    if (!messagesObj.contains("delay")) {
        return {};
    }
    nlohmann::json::array_t delayArr;
    messagesObj.at("delay").get_to(delayArr);

    std::vector<std::string> result{};
    for (const nlohmann::json& jMsg : delayArr) {
        if (!jMsg.contains("text")) {
            SPDLOG_WARN("Failed to parse departure delay messages. 'text' filed missing. Skipping...");
            continue;
        }
        std::string text;
        jMsg.at("text").get_to(text);
        if (text.empty()) {
            SPDLOG_WARN("Failed to parse departure delay messages. 'text' filed empty. Skipping...");
            continue;
        }
        result.emplace_back(text);
    }
    return result;
}

std::shared_ptr<Departure> Departure::from_json(const nlohmann::json& j) {
    if (!j.contains("departure")) {
        SPDLOG_ERROR("Failed to parse departure. 'departure' filed missing.");
        return nullptr;
    }
    nlohmann::json departureObj;
    j.at("departure").get_to(departureObj);

    if (!departureObj.contains("scheduledTime")) {
        SPDLOG_ERROR("Failed to parse departure. 'scheduledTime' filed missing.");
        return nullptr;
    }
    std::string depTimeScheduledStr;
    departureObj.at("scheduledTime").get_to(depTimeScheduledStr);
    std::istringstream depTimeScheduledStrSs{depTimeScheduledStr};
    std::chrono::system_clock::time_point depTimeScheduled;
    depTimeScheduledStrSs >> date::parse("%Y-%m-%dT%T.000%Z", depTimeScheduled);

    if (!departureObj.contains("time")) {
        SPDLOG_ERROR("Failed to parse departure. 'time' filed missing.");
        return nullptr;
    }
    std::string depTimeStr;
    departureObj.at("time").get_to(depTimeStr);
    std::istringstream depTimeStrSs{depTimeStr};
    std::chrono::system_clock::time_point depTime;
    depTimeStrSs >> date::parse("%Y-%m-%dT%T.000%Z", depTime);

    if (!departureObj.contains("platform")) {
        SPDLOG_ERROR("Failed to parse departure. 'platform' filed missing.");
        return nullptr;
    }
    std::string platform;
    departureObj.at("platform").get_to(platform);

    if (!departureObj.contains("scheduledPlatform")) {
        SPDLOG_ERROR("Failed to parse departure. 'scheduledPlatform' filed missing.");
        return nullptr;
    }
    std::string platformScheduled;
    departureObj.at("scheduledPlatform").get_to(platformScheduled);

    if (!departureObj.contains("delay")) {
        SPDLOG_ERROR("Failed to parse departure. 'delay' filed missing.");
        return nullptr;
    }
    int delay{0};
    departureObj.at("delay").get_to(delay);

    if (!departureObj.contains("cancelled")) {
        SPDLOG_ERROR("Failed to parse departure. 'cancelled' filed missing.");
        return nullptr;
    }
    bool cancelled = false;
    departureObj.at("cancelled").get_to(cancelled);

    if (!j.contains("productClass")) {
        SPDLOG_ERROR("Failed to parse departure. 'productClass' filed missing.");
        return nullptr;
    }
    std::string productClass;
    j.at("productClass").get_to(productClass);

    if (!j.contains("scheduledDestination")) {
        SPDLOG_ERROR("Failed to parse departure. 'scheduledDestination' filed missing.");
        return nullptr;
    }
    std::string destination;
    j.at("scheduledDestination").get_to(destination);

    if (!j.contains("train")) {
        SPDLOG_ERROR("Failed to parse departure. 'train' filed missing.");
        return nullptr;
    }
    nlohmann::json trainObj;
    j.at("train").get_to(trainObj);

    if (!trainObj.contains("name")) {
        SPDLOG_ERROR("Failed to parse departure. 'name' filed missing.");
        return nullptr;
    }
    std::string trainName;
    trainObj.at("name").get_to(trainName);

    std::vector<std::string> infoMessages = parse_info_messages(j);

    return std::make_shared<Departure>(Departure{
        depTime,
        depTimeScheduled,
        platform,
        platformScheduled,
        delay,
        cancelled,
        productClass,
        trainName,
        destination,
        std::move(infoMessages)});
}
}  // namespace backend::db
