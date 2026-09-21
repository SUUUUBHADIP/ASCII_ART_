#pragma once
#include "AsciiConverter.hpp"
#include <string>
class Renderer {
public:
    static void renderToTerminal(const AsciiFrame&, bool color);
    static bool renderToFile(const AsciiFrame&, const std::string&, bool color);
};
