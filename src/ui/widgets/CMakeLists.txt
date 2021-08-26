cmake_minimum_required(VERSION 3.16)

add_library(ui_widgets STATIC MvgWidget.cpp
                              MvgWidget.hpp
                              WeatherWidget.cpp
                              WeatherWidget.hpp
                              ActionsWidget.cpp
                              ActionsWidget.hpp
                              DepartureWidget.cpp
                              DepartureWidget.hpp
                              SettingsWidget.cpp
                              SettingsWidget.hpp
                              DeviceStatusWidget.cpp
                              DeviceStatusWidget.hpp)
target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_mvg backend_weather backend_deviceStatus
                                 PRIVATE logger backend_storage ui_utils)
