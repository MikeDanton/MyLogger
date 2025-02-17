#ifndef LOGGER_MODULES_HPP
#define LOGGER_MODULES_HPP

#include "color_module.hpp"

class LoggerModules {
private:
    ColorModule colorModule;  // ✅ Private to prevent direct access

public:
    ColorModule& getColorModule() { return colorModule; }  // ✅ Public getter
};

#endif // LOGGER_MODULES_HPP
