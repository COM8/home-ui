#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::weather {
struct Temp {
    double min;
    double max;
    double day;
    double night;
    double eve;
    double morn;

    static Temp from_json(const nlohmann::json& j);
} __attribute__((aligned(64)));

struct Weather {
    int id;
    std::string main;
    std::string description;
    std::string icon;

    static Weather from_json(const nlohmann::json& j);
} __attribute__((aligned(128)));

struct Day {
    std::chrono::system_clock::time_point time;
    Temp temp;
    Weather weather;
    double rain;

    static Day from_json(const nlohmann::json& j);
};

struct Hour {
    std::chrono::system_clock::time_point time;
    double temp;
    double feelsLike;
    Weather weather;

    static Hour from_json(const nlohmann::json& j);
};

struct Forecast {
    double temp;
    double feelsLike;
    Weather weather;

    std::vector<Day> daily;
    std::vector<Hour> hourly;

    static Forecast from_json(const nlohmann::json& j);
} __attribute__((aligned(64)));
}  // namespace backend::weather