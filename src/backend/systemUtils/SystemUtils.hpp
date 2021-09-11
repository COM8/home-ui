#pragma once

#include <cstdint>
#include <string>

namespace backend::systemUtils {
void activate_screensaver();
uint8_t get_screen_brightness();
void set_screen_brightness(uint8_t brightness);
}  // namespace backend::systemUtils