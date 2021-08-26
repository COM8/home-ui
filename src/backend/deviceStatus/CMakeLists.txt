cmake_minimum_required(VERSION 3.16)

add_library(backend_deviceStatus STATIC DeviceStatus.cpp
                                        DeviceStatus.hpp)

target_link_libraries(backend_deviceStatus PRIVATE logger)
