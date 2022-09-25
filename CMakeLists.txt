cmake_minimum_required(VERSION 3.16)

set(PROJECT_DESCRIPTION "GTK3 Based UI for home automation using a Raspberry Pi. ")

project("home_ui" VERSION 1.0.0
                  DESCRIPTION "${PROJECT_DESCRIPTION}"
                  HOMEPAGE_URL "https://github.com/COM8/home-ui")
set(VERSION_NAME "dev")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

# gtk4 stuff
include(glib-resource-compiler)
find_package(PkgConfig REQUIRED)

# Additional search path since gtkmm-4.0 will usually be installed in /usr
set(ENV{PKG_CONFIG_PATH} "/usr/lib/pkgconfig")
pkg_check_modules(GTKMM gtkmm-4.0>=4.4.0 REQUIRED IMPORTED_TARGET)
pkg_check_modules(EPOXY epoxy REQUIRED IMPORTED_TARGET)

# Lib adwaita
pkg_check_modules(ADWAITA REQUIRED libadwaita-1>=1.1.0)
include_directories(${ADWAITA_INCLUDE_DIRS})

# libshumate
include(ExternalProject)
find_program(MESON_PATH meson REQUIRED)

if(MESON_PATH STREQUAL "MESON_PATH-NOTFOUND")
    message(FATAL_ERROR "meson not found. Please make sure you have meson installed on your system.")
    return()
endif()

ExternalProject_Add(shumate_build GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libshumate.git
                                  GIT_TAG main
                                  CONFIGURE_COMMAND ${MESON_PATH} "<BINARY_DIR>" "<SOURCE_DIR>" -Dgtk_doc=false
                                  BUILD_COMMAND cd "<BINARY_DIR>" && ninja
                                  CONFIGURE_HANDLED_BY_BUILD ON
                                  INSTALL_COMMAND ""
                                  COMMENT "Building shumate"
                                  BUILD_BYPRODUCTS "<BINARY_DIR>/shumate/libshumate-1.0.so")

add_library(libshumate SHARED IMPORTED)
add_dependencies(libshumate shumate_build)

ExternalProject_Get_Property(shumate_build BINARY_DIR)
set(SHUMATE_LIB "${BINARY_DIR}/shumate/libshumate-1.0.so")
set_target_properties(libshumate PROPERTIES IMPORTED_LOCATION ${SHUMATE_LIB})

ExternalProject_Get_Property(shumate_build SOURCE_DIR)
set(SHUMATE_INCLUDE_DIRS "${SOURCE_DIR}" "${BINARY_DIR}")
target_include_directories(libshumate INTERFACE "${SHUMATE_INCLUDE_DIRS}")
include_directories(${SHUMATE_INCLUDE_DIRS})

# Disable linting for fetch content projects
clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG fe111747b01ab922e2c5e4fe047c3c15084fa75c)
set(CPR_BUILD_TESTS OFF CACHE INTERNAL " " FORCE)
set(CURL_CA_FALLBACK ON CACHE INTERNAL " " FORCE) # Workaround for failing SSL requests
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git
                          GIT_TAG v3.10.5)
FetchContent_MakeAvailable(json)

set(SPDLOG_INSTALL ON)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
                            GIT_TAG v1.9.2)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(re2 GIT_REPOSITORY https://github.com/google/re2.git
                         GIT_TAG 2022-06-01)
FetchContent_MakeAvailable(re2)

restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)