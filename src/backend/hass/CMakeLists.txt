cmake_minimum_required(VERSION 3.16)

add_library(backend_hass STATIC HassHelper.cpp
                                HassHelper.hpp)

target_link_libraries(backend_hass PRIVATE logger cpr::cpr
                                   PUBLIC nlohmann_json::nlohmann_json)
