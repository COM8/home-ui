cmake_minimum_required(VERSION 3.16)

add_library(backend_lightning STATIC LightningHelper.cpp
                                     LightningHelper.hpp
                                     Lightning.cpp
                                     Lightning.hpp)

target_link_libraries(backend_lightning PRIVATE logger ixwebsocket
                                        PUBLIC nlohmann_json::nlohmann_json eventpp::eventpp)
