#ifndef COLOR_MODULE_HPP
#define COLOR_MODULE_HPP

#include <string>
#include <unordered_map>

class ColorModule {
public:
    static std::string getColorCode(const std::string& key, const std::unordered_map<std::string, int>& colorMap);
};

#endif // COLOR_MODULE_HPP
