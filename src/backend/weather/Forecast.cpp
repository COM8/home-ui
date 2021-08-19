#include "Forecast.hpp"
#include <nlohmann/json.hpp>

namespace backend::weather {
Forecast Forecast::from_json(const nlohmann::json& j) {
    const nlohmann::json current = j["current"];
    double temp = 0;
    current.at("temp").get_to(temp);
    double feelsLike = 0;
    current.at("feels_like").get_to(feelsLike);

    std::vector<Day> daily{};
    nlohmann::json::array_t dailyJson;
    j.at("daily").get_to(dailyJson);
    for (const nlohmann::json& dayJson : dailyJson) {
        daily.push_back(Day::from_json(dayJson));
    }

    std::vector<Hour> hourly{};
    nlohmann::json::array_t hourlyJson;
    j.at("hourly").get_to(hourlyJson);
    for (const nlohmann::json& hourJson : hourlyJson) {
        hourly.push_back(Hour::from_json(hourJson));
    }

    return Forecast{
        temp,
        feelsLike,
        Weather::from_json(current["weather"][0]),
        std::move(daily),
        std::move(hourly)};
}

Hour Hour::from_json(const nlohmann::json& j) {
    size_t time = 0;
    j.at("dt").get_to(time);
    std::chrono::system_clock::time_point timeTp = std::chrono::system_clock::from_time_t(static_cast<time_t>(time));

    double temp = 0;
    j.at("temp").get_to(temp);
    double feelsLike = 0;
    j.at("feels_like").get_to(feelsLike);

    return Hour{
        timeTp,
        temp,
        feelsLike,
        Weather::from_json(j["weather"][0])};
}

Day Day::from_json(const nlohmann::json& j) {
    size_t time = 0;
    j.at("dt").get_to(time);
    std::chrono::system_clock::time_point timeTp = std::chrono::system_clock::from_time_t(static_cast<time_t>(time));

    double rain = 0;
    if (j.contains("rain")) {
        j.at("rain").get_to(rain);
    }

    return Day{
        timeTp,
        Temp::from_json(j["temp"]),
        Weather::from_json(j["weather"][0]),
        rain};
}

Weather Weather::from_json(const nlohmann::json& j) {
    int id = -1;
    j.at("id").get_to(id);
    std::string main;
    j.at("main").get_to(main);
    std::string description;
    j.at("description").get_to(description);
    std::string icon;
    j.at("icon").get_to(icon);

    return Weather{
        id,
        main,
        description,
        icon};
}

Temp Temp::from_json(const nlohmann::json& j) {
    double min = 0;
    j.at("min").get_to(min);
    double max = 0;
    j.at("max").get_to(max);
    double day = 0;
    j.at("day").get_to(day);
    double night = 0;
    j.at("night").get_to(night);
    double eve = 0;
    j.at("eve").get_to(eve);
    double morn = 0;
    j.at("morn").get_to(morn);
    return Temp{
        min,
        max,
        day,
        night,
        eve,
        morn};
}
}  // namespace backend::weather