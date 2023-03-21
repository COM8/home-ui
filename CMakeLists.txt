cmake_minimum_required(VERSION 3.16)

set(PROJECT_DESCRIPTION "GTK3 Based UI for home automation using a Raspberry Pi. ")

project("home_ui" VERSION 1.0.0
                  DESCRIPTION "${PROJECT_DESCRIPTION}"
                  HOMEPAGE_URL "https://github.com/COM8/home-ui")
set(VERSION_NAME "dev")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

macro(home_ui_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "C++ Home UI Options")
message(STATUS "=======================================================")
home_ui_option(HOME_UI_STATIC_ANALYZE "Set to ON to enable the GCC 10 static analysis. If enabled, HOME_UI_ENABLE_LINTING has to be disabled." OFF)
home_ui_option(HOME_UI_ENABLE_LINTING "Set to ON to enable clang linting. If enabled, HOME_UI_STATIC_ANALYZE has to be disabled." OFF)
home_ui_option(HOME_UI_FLATPAK_BUILD "Set to ON in case it is built for flatpak." OFF)
message(STATUS "=======================================================")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(sanitizer)
include(gcc_analyze)
include(glib-resource-compiler)
include(clear_variable)
include(FetchContent)

if(${HOME_UI_ENABLE_LINTING})
    message(STATUS "Enabling linting")
    include(clang-tidy)
else()
    message(STATUS "Linting is disabled")
endif()

if(HOME_UI_FLATPAK_BUILD)
    add_compile_definitions(HOME_UI_FLATPAK_BUILD=1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

# gtk4 stuff
include(glib-resource-compiler)
find_package(PkgConfig REQUIRED)

# Additional search path since gtkmm-4.0 will usually be installed in /usr
set(ENV{PKG_CONFIG_PATH} "/app/lib/pkgconfig")
pkg_check_modules(GTKMM gtkmm-4.0>=4.4.0 REQUIRED IMPORTED_TARGET)
pkg_check_modules(EPOXY epoxy REQUIRED IMPORTED_TARGET)

# Lib adwaita
pkg_check_modules(ADWAITA REQUIRED libadwaita-1>=1.1.0)
include_directories(${ADWAITA_INCLUDE_DIRS})

# libshumate
pkg_check_modules(SHUMATE REQUIRED shumate-1.0>=1.0.0)
include_directories(${SHUMATE_INCLUDE_DIRS})

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(ixwebsocket CONFIG REQUIRED)
find_package(eventpp CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)
