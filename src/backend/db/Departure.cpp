#include "Departure.hpp"
#include "logger/Logger.hpp"
#include "nlohmann/json.hpp"
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <date/date.h>
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

std::string get_destination(const std::vector<Stop>& nextStops) {
    assert(!nextStops.empty());

    for (size_t i = nextStops.size(); i >= 1; i--) {
        if (!nextStops[i - 1].canceled) {
            return nextStops[i - 1].name;
        }
    }
    return nextStops[nextStops.size() - 1].name;
}

std::tuple<std::vector<Stop>, std::vector<Stop>> parse_stops(const nlohmann::json& j, const std::string& curStopPlace) {
    std::vector<Stop> prevStops;
    std::vector<Stop> nextStops;

    if (!j.contains("route")) {
        SPDLOG_ERROR("Failed to parse departure. 'route' filed missing.");
        return {};
    }
    nlohmann::json::array_t routeArr;
    j.at("route").get_to(routeArr);

    bool nextStop = false;
    for (const nlohmann::json& jStop : routeArr) {
        if (!jStop.contains("name")) {
            SPDLOG_WARN("Failed to parse departure route stop. 'name' filed missing. Skipping...");
            continue;
        }
        std::string name;
        jStop.at("name").get_to(name);

        bool cancelled = false;
        if (jStop.contains("cancelled")) {
            jStop.at("cancelled").get_to(cancelled);
        }

        bool showVia = false;
        if (jStop.contains("showVia")) {
            jStop.at("showVia").get_to(showVia);
        }

        if (nextStop || name == curStopPlace) {
            nextStop = true;
            nextStops.emplace_back(Stop{name, cancelled, showVia});
        } else {
            prevStops.emplace_back(Stop{name, cancelled, showVia});
        }
    }

    return std::make_tuple(std::move(prevStops), std::move(nextStops));
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

    int delay{0};
    if (departureObj.contains("delay")) {
        departureObj.at("delay").get_to(delay);
    }

    bool cancelled = false;
    if (departureObj.contains("cancelled")) {
        departureObj.at("cancelled").get_to(cancelled);
    }

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
    std::string destinationScheduled;
    j.at("scheduledDestination").get_to(destinationScheduled);

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

    if (!j.contains("currentStopPlace")) {
        SPDLOG_ERROR("Failed to parse departure. 'currentStopPlace' filed missing.");
        return nullptr;
    }
    nlohmann::json curStopPlaceObj;
    j.at("currentStopPlace").get_to(curStopPlaceObj);

    if (!curStopPlaceObj.contains("name")) {
        SPDLOG_ERROR("Failed to parse departure. 'name' filed missing.");
        return nullptr;
    }
    std::string curStopPlace;
    curStopPlaceObj.at("name").get_to(curStopPlace);

    std::vector<std::string> infoMessages = parse_info_messages(j);

    std::tuple<std::vector<Stop>, std::vector<Stop>> prevNextStops = parse_stops(j, curStopPlace);
    std::string destination = get_destination(std::get<1>(prevNextStops));

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
        destinationScheduled,
        curStopPlace,
        std::get<0>(prevNextStops),
        std::get<1>(prevNextStops),
        std::move(infoMessages)});
}
}  // namespace backend::db
