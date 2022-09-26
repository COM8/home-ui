cmake_minimum_required(VERSION 3.16)

add_library(backend_lightning STATIC LightningHelper.cpp
                                     LightningHelper.hpp)

target_link_libraries(backend_lightning PRIVATE logger websocketpp::websocketpp
                                        PUBLIC nlohmann_json::nlohmann_json)
