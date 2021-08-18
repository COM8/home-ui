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
                              SettingsWidget.hpp)
target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_mvg backend_weather
                                 PRIVATE logger backend_storage)
