cmake_minimum_required(VERSION 3.16)

add_library(backend_db Departure.cpp
                       Departure.hpp
                       DbHelper.cpp
                       DbHelper.hpp)

target_link_libraries(backend_db PRIVATE logger cpr::cpr date::date date::date-tz
                                  PUBLIC nlohmann_json::nlohmann_json)
