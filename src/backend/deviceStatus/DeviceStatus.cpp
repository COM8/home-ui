#include "DeviceStatus.hpp"
#include <cstdlib>

namespace backend::deviceStatus {
bool ping(const std::string& ip) {
    const std::string command = "ping -c2 -s1 " + ip + "  > /dev/null 2>&1";
    return std::system(command.c_str()) == 0;
}
}  // namespace backend::deviceStatus