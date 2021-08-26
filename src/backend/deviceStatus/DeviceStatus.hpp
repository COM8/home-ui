#pragma once

#include <string>

namespace backend::deviceStatus {
bool ping(const std::string& ip);
}  // namespace backend::deviceStatus