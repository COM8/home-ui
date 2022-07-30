cmake_minimum_required(VERSION 3.16)

add_library(backend_storage STATIC Serializer.cpp
                                   Serializer.hpp
                                   Settings.cpp
                                   Settings.hpp)

target_link_libraries(backend_storage PUBLIC logger nlohmann_json::nlohmann_json)
                              
target_compile_features(backend_storage PRIVATE cxx_std_17 cxx_constexpr)
