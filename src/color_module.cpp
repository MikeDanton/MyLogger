#include "color_module.hpp"

std::string ColorModule::getColorCode(const std::string& key, const std::unordered_map<std::string, int>& colorMap) {
    auto it = colorMap.find(key);
    int colorValue = (it != colorMap.end()) ? it->second : 37;
    return "\033[" + std::to_string(colorValue) + "m";
}
