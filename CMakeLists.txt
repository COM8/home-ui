cmake_minimum_required(VERSION 3.16)

set(PROJECT_DESCRIPTION "GTK3 Based UI for home automation using a Raspberry Pi. ")

project("Home-UI" 
            VERSION 0.0.0
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
home_ui_option(HOME_UI_BUILD_TESTS "Set to ON to build tests." ON)
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

if (${HOME_UI_ENABLE_LINTING})
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

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CONAN_CONFIGS"Release;Debug;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE IN_LIST CONAN_CONFIGS)
    set(CONAN_BUILD_TYPE "Debug")
else()
    set(CONAN_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

# Prevent an unknown build type for conan:
clear_variable(DESTINATION CMAKE_BUILD_TYPE BACKUP CMAKE_BUILD_TYPE_BKP REPLACE ${CONAN_BUILD_TYPE})

conan_cmake_run(REQUIRES catch2/2.13.9
                         nlohmann_json/3.9.1
                         spdlog/1.8.5
                         re2/20210601
                GENERATORS cmake_find_package
                BUILD missing)

conan_cmake_autodetect(settings build_type ${CONAN_BUILD_TYPE})
restore_variable(DESTINATION CMAKE_BUILD_TYPE BACKUP CMAKE_BUILD_TYPE_BKP)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

find_package(Catch2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(re2 REQUIRED)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG db351ffbbadc6c4e9239daaa26e9aefa9f0ec82d) # The commit hash for 1.8.3. Replace with the latest from: https://github.com/libcpr/cpr/releases
set(CPR_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(CURL_CA_FALLBACK ON CACHE INTERNAL "" FORCE) # Workaround for failing SSL requests
FetchContent_MakeAvailable(cpr)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

# Testing
if(${HOME_UI_BUILD_TESTS})
    message(STATUS "Testing is enabled")
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Testing is disabled")
endif()

