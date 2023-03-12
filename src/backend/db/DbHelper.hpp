#pragma once

#include "Departure.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace backend::db {
std::vector<std::shared_ptr<Departure>> request_departures(const std::string& stationId, size_t lookAhead, size_t lookBehind);
}  // namespace backend::db