#include "SystemUtils.hpp"
#include "logger/Logger.hpp"
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <spdlog/spdlog.h>

namespace backend::systemUtils {
void activate_screensaver() {
#ifdef HOME_UI_FLATPAK_BUILD
    const std::filesystem::path xdgPath = "/var/run/host/usr/bin/xdg-screensaver activate";
#else
    const std::filesystem::path xdgPath = "/usr/bin/xdg-screensaver activate";
#endif  // HOME_UI_FLATPAK_BUILD

    // NOLINTNEXTLINE (cert-env33-c, concurrency-mt-unsafe)
    int result = std::system(xdgPath.c_str());
    SPDLOG_INFO("Screen locked with: {}", result);
}

uint8_t get_screen_brightness() {
#ifdef HOME_UI_FLATPAK_BUILD
    const std::filesystem::path brightnessPath = "/var/run/host/sys/class/backlight/rpi_backlight/brightness";
#else
    const std::filesystem::path brightnessPath = "/sys/class/backlight/rpi_backlight/brightness";
#endif  // HOME_UI_FLATPAK_BUILD

    std::ifstream file(brightnessPath);
    if (file.is_open()) {
        try {
            std::string brightnessStr;
            if (file >> brightnessStr) {
                file.close();
                SPDLOG_DEBUG("Read screen brightness: {}", brightnessStr);
                return static_cast<uint8_t>(std::stoul(brightnessStr));
            }
            SPDLOG_ERROR("Invalid screen brightness file content.");
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to read from screen brightness file with: {}", e.what());
        }
        file.close();
    }
    SPDLOG_ERROR("Failed to open screen brightness file for reading.");
    return 128;
}

void set_screen_brightness(uint8_t brightness) {
    std::ofstream file("/sys/class/backlight/rpi_backlight/brightness");
    if (file.is_open()) {
        try {
            if (file << std::to_string(brightness)) {
                file.close();
                SPDLOG_DEBUG("Wrote screen brightness: {}", brightness);
                return;
            }
            SPDLOG_ERROR("Failed to write screen brightness to file.");
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to write to screen brightness file with: {}", e.what());
        }
        file.close();
    }
    SPDLOG_ERROR("Failed to open screen brightness file for writing.");
}
}  // namespace backend::systemUtils