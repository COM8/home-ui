cmake_minimum_required(VERSION 3.16)

add_library(ui_widgets STATIC DbWidget.cpp
                              DbWidget.hpp
                              WeatherWidget.cpp
                              WeatherWidget.hpp
                              ActionsWidget.cpp
                              ActionsWidget.hpp
                              DepartureWidget.cpp
                              DepartureWidget.hpp
                              SettingsWidget.cpp
                              SettingsWidget.hpp
                              DeviceStatusWidget.cpp
                              DeviceStatusWidget.hpp
                              LightWidget.cpp
                              LightWidget.hpp
                              LightningMap.cpp
                              LightningMap.hpp
                              LightningWidget.cpp
                              LightningWidget.hpp)
target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_db backend_weather backend_deviceStatus backend_hass
                                 PRIVATE logger backend_storage ui_utils re2::re2 libshumate backend_lightning backend_date)
