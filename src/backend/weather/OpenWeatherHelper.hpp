#pragma once

#include "Forecast.hpp"
#include <memory>
#include <string>

namespace backend::weather {
std::shared_ptr<Forecast> request_weather(const std::string& lat, const std::string& lon, const std::string& apiKey);
}  // namespace backend::weather