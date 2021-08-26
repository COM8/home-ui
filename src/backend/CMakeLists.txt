cmake_minimum_required(VERSION 3.16)

add_subdirectory(storage)
add_subdirectory(mvg)
add_subdirectory(weather)
add_subdirectory(deviceStatus)

# add_library(backend STATIC)
# target_link_libraries(backend PRIVATE logger PkgConfig::GTKMM)
# install(TARGETS backend)
