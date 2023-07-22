#include "util.h"

namespace util
{
    bool match(uint16_t ins, uint16_t a, uint16_t x, uint16_t y, uint16_t n)
    {
        // TODO duplicate work, fix this
        uint8_t ai = (ins & 0xF000) >> 12;
        uint8_t xi = (ins & 0x0F00) >> 8;
        uint8_t yi = (ins & 0x00F0) >> 4;
        uint8_t ni = ins & 0x000F;

        return (a == ai)               // does the first nibble match
               && (x == NA || x == xi) // do the other nibbles match, or are they irrelevant?
               && (y == NA || y == yi) && (n == NA || n == ni);
    }

    void log(std::string msg)
    {
        fmt::print("{}\n", msg);
    }

    void info(std::string msg)
    {
        fmt::print(fmt::fg(fmt::color::green), "{}\n", msg);
    }

    void warning(std::string msg)
    {
        fmt::print(fmt::fg(fmt::color::yellow), "{}\n", msg);
    }

    void error(std::string msg)
    {
        fmt::print(fmt::fg(fmt::color::red), "{}\n", msg);
    }
}