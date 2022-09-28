cmake_minimum_required(VERSION 3.16)

add_library(backend_systemUtils STATIC SystemUtils.cpp
                                       SystemUtils.hpp)

target_link_libraries(backend_systemUtils PRIVATE logger)
