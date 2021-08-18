cmake_minimum_required(VERSION 3.16)

add_library(backend_weather STATIC OpenWeatherHelper.cpp
                                   OpenWeatherHelper.hpp
                                   Forecast.cpp
                                   Forecast.hpp)

target_link_libraries(backend_weather PRIVATE logger cpr::cpr
                                      PUBLIC nlohmann_json::nlohmann_json)
