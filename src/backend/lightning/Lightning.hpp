#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace backend::lightning {
struct Lightning {
    double lat{0};
    double lon{0};
    std::chrono::system_clock::time_point time;

    static std::optional<Lightning> from_json(const nlohmann::json& j);
    [[nodiscard]] double distance(double lat, double lon) const;
} __attribute__((aligned(32)));
}  // namespace backend::lightning