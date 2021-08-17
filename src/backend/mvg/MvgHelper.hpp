#pragma once

#include "Departure.hpp"
#include <memory>
#include <vector>

namespace backend::mvg {
std::vector<std::shared_ptr<Departure>> request_departures(const std::string& stationId, bool bus, bool ubahn, bool sbahn, bool tram);
}  // namespace backend::mvg