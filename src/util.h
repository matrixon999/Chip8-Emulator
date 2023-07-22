#ifndef UTIL
#define UTIL

#include <iostream>
#include <string>
#include <fmt/color.h>

static const uint16_t NA = -1;

namespace util
{
    bool match(uint16_t ins, uint16_t a, uint16_t x, uint16_t y, uint16_t n);
    void log(std::string msg);
    void info(std::string msg);
    void warning(std::string msg);
    void error(std::string msg);
}

#endif