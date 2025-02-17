#ifndef COLOR_MODULE_HPP
#define COLOR_MODULE_HPP

#include <string>
#include <unordered_map>
#include <string_view>

class ColorModule {
public:
    explicit ColorModule() = default;  // No singleton, dependency-injected instead
    void setColorMap(std::unordered_map<std::string, std::string> colors);
    std::string getColor(std::string_view key) const;

    std::unordered_map<std::string, std::string> colorMap;  // Stores colors as `#RRGGBBAA`
};

#endif // COLOR_MODULE_HPP
