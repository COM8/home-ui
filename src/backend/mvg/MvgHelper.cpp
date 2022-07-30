#include "MvgHelper.hpp"
#include "cpr/body.h"
#include "logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <cpr/session.h>
#include <spdlog/spdlog.h>

namespace backend::mvg {
cpr::Url build_url(const std::string& stationId, bool bus, bool ubahn, bool sbahn, bool tram) {
    return cpr::Url("https://www.mvg.de/api/fahrinfo/departure/" + stationId + "?footway=0" + "&bus=" + (bus ? "true" : "false") + "&ubahn=" + (ubahn ? "true" : "false") + "&sbahn=" + (sbahn ? "true" : "false") + "&tram=" + (tram ? "true" : "false"));
}

std::vector<std::shared_ptr<Departure>> parse_response(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);

        // Departures:
        if (!j.contains("departures")) {
            SPDLOG_ERROR("Failed to parse departures. 'departures' filed missing.");
            SPDLOG_DEBUG("Response: {}", response);
            return std::vector<std::shared_ptr<Departure>>();
        }

        nlohmann::json::array_t array;
        j.at("departures").get_to(array);

        std::vector<std::shared_ptr<Departure>> result{};
        for (const nlohmann::json& jDep : array) {
            std::shared_ptr<Departure> dep = Departure::from_json(jDep);
            if (dep) {
                result.push_back(std::move(dep));
            } else {
                SPDLOG_DEBUG("Departure: {}", jDep.dump());
            }
        }
        SPDLOG_DEBUG("Found {} departures.", result.size());
        return result;

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing departures from '{}' with: {}", response, e.what());
    }
    return std::vector<std::shared_ptr<Departure>>();
}

std::vector<std::shared_ptr<Departure>> request_departures(const std::string& stationId, bool bus, bool ubahn, bool sbahn, bool tram) {
    cpr::Session session;
    session.SetUrl(build_url(stationId, bus, ubahn, sbahn, tram));

    SPDLOG_DEBUG("Requesting departure times for station '{}'...", stationId);
    cpr::Response response = session.Get();
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting departures failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting departures failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return std::vector<std::shared_ptr<Departure>>();
    }
    SPDLOG_DEBUG("Departures requested successfully. Parsing...");
    return parse_response(response.text);
}
}  // namespace backend::mvg
