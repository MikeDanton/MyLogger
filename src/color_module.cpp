#include "color_module.hpp"
#include <iostream>

void ColorModule::setColorMap(std::unordered_map<std::string, std::string> colors) {
    std::cerr << "[DEBUG] Setting color map. Size: " << colors.size() << "\n";
    for (const auto& [key, value] : colors) {
        std::cerr << "[DEBUG] Storing Color: " << key << " -> " << value << "\n";
    }
    colorMap = std::move(colors);
}

std::string ColorModule::getColor(std::string_view key) const {
    auto it = colorMap.find(std::string(key));
    if (it != colorMap.end()) {
        return it->second;
    }
    return "#FFFFFF";  // Default: White (opaque)
}
