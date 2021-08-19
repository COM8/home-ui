cmake_minimum_required(VERSION 3.16)

set(RESOURCE_DEF ${CMAKE_CURRENT_SOURCE_DIR}/ui_resources.c.xml PARENT_SCOPE)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(RESOURCE_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/theme.css
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/01n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/02n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/03n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/04n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/09n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/10n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/11n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/13n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/50n.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/01d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/02d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/03d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/04d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/09d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/10d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/11d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/13d.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/openWeather/4x/50d.png
     PARENT_SCOPE)