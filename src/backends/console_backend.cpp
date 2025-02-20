#include "console_backend.hpp"
#include "logger_config.hpp"

struct RGB {
    int r, g, b;
};

// ANSI 16-color palette (foreground colors)
const std::array<std::pair<int, RGB>, 8> ANSI_COLORS = {{
    {30, {0, 0, 0}},        // Black
    {31, {255, 0, 0}},      // Red
    {32, {0, 255, 0}},      // Green
    {33, {255, 215, 0}},    // Yellow (Gold)
    {34, {0, 0, 255}},      // Blue
    {35, {138, 43, 226}},   // Magenta (BlueViolet)
    {36, {135, 206, 250}},  // Cyan (Light Sky Blue)
    {37, {255, 255, 255}}   // White
}};

//------------------------------------------------------------------------------
// Convert Hex Color to RGB
//------------------------------------------------------------------------------
RGB hexToRGB(const std::string& hexColor) {
    if (hexColor.size() != 9 || hexColor[0] != '#') {
        return {255, 255, 255}; // Default to white if invalid
    }

    int r, g, b;
    std::stringstream ss;
    ss << std::hex << hexColor.substr(1, 2) << " "
       << hexColor.substr(3, 2) << " "
       << hexColor.substr(5, 2);
    ss >> r >> g >> b;

    return {r, g, b};
}

//------------------------------------------------------------------------------
// Compute Euclidean Distance
//------------------------------------------------------------------------------
double colorDistance(const RGB& c1, const RGB& c2) {
    return std::sqrt(
        (c1.r - c2.r) * (c1.r - c2.r) +
        (c1.g - c2.g) * (c1.g - c2.g) +
        (c1.b - c2.b) * (c1.b - c2.b)
    );
}

//------------------------------------------------------------------------------
// Find Closest ANSI Color
//------------------------------------------------------------------------------
int ConsoleBackend::hexToAnsiColor(const std::string& hexColor) {
    RGB inputColor = hexToRGB(hexColor);

    int closestAnsi = 37; // Default to white
    double minDistance = std::numeric_limits<double>::max();

    for (const auto& [ansi, color] : ANSI_COLORS) {
        double dist = colorDistance(inputColor, color);
        if (dist < minDistance) {
            minDistance = dist;
            closestAnsi = ansi;
        }
    }
    return closestAnsi;
}

//------------------------------------------------------------------------------
// Setup Console Logging
//------------------------------------------------------------------------------
void ConsoleBackend::setup(const LoggerSettings& settings) {

    colorMode = settings.config.colors.colorMode;
    hideLevelTag = settings.config.display.hideLevelTag;
    hideContextTag = settings.config.display.hideContextTag;
}

//------------------------------------------------------------------------------
// Write Log Message to Console with Applied Settings
//------------------------------------------------------------------------------
void ConsoleBackend::write(const LogMessage& log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    auto& colors = settings.config.colors;
    auto& display = settings.config.display;

    // ✅ Fetch the latest settings directly from LoggerSettings
    std::string colorMode = colors.colorMode;
    bool hideLevelTag = display.hideLevelTag;
    bool hideContextTag = display.hideContextTag;

    std::string key = (colorMode == "context") ? "context_" + std::string(log.context)
                                               : "level_" + std::string(log.level);

    std::string hexColor = colors.parsedLogColors.contains(key) ? colors.parsedLogColors.at(key) : "#FFFFFF";
    int ansiColor = hexToAnsiColor(hexColor);
    std::string ansiCode = "\033[" + std::to_string(ansiColor) + "m";

    std::string levelTag = hideLevelTag ? "" : ("[" + std::string(log.level) + "] ");
    std::string contextTag = hideContextTag ? "" : (std::string(log.context) + ": ");

    std::cout << ansiCode << log.timestamp << " " << levelTag << contextTag << log.message << reset << "\n";
}

//------------------------------------------------------------------------------
// Flush Console Output
//------------------------------------------------------------------------------
void ConsoleBackend::flush() {
    std::cout.flush();
}

//------------------------------------------------------------------------------
// Shutdown Console Backend
//------------------------------------------------------------------------------
void ConsoleBackend::shutdown() {
    flush();
}
