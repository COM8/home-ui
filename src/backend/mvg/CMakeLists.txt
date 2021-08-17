cmake_minimum_required(VERSION 3.16)

add_library(backend_mvg STATIC Departure.cpp
                               Departure.hpp
                               MvgHelper.cpp
                               MvgHelper.hpp)

target_link_libraries(backend_mvg PRIVATE logger cpr::cpr
                                  PUBLIC nlohmann_json::nlohmann_json)
