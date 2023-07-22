#ifndef CHIP_8
#define CHIP_8

#include <stdint.h>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <iomanip>
#include <random>
#include <string>
#include <bitset>
#include <array>
#include <vector>
#include <optional>

#include "util.h"

static const uint8_t FONT_DATA[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

struct Opcode {
    std::string mnemonic;
};

struct OpcodeEntry {
    std::array<uint16_t, 4> key;
    Opcode opcode;
};

static const std::vector<OpcodeEntry> OPCODES = {
    {{0x0, 0x0, 0xE, 0x0}, Opcode {"CLS"}},
    {{0x0, 0x0, 0xE, 0xE}, Opcode {"RET"}},
    {{0x0, NA, NA, NA}, Opcode {"SYS addr"}},
    {{0x1, NA, NA, NA}, Opcode {"JP addr"}},
    {{0x2, NA, NA, NA}, Opcode {"CALL addr"}},
    {{0x3, NA, NA, NA}, Opcode {"SE Vx, byte"}},
    {{0x4, NA, NA, NA}, Opcode {"SNE Vx, byte"}},
    {{0x5, NA, NA, 0x0}, Opcode {"SE Vx, Vy"}},
    {{0x6, NA, NA, NA}, Opcode {"LD Vx, byte"}},
    {{0x7, NA, NA, NA}, Opcode {"ADD Vx, byte"}},
    {{0x8, NA, NA, 0x0}, Opcode {"LD Vx, Vy"}},
    {{0x8, NA, NA, 0x1}, Opcode {"OR Vx, Vy"}},
    {{0x8, NA, NA, 0x2}, Opcode {"AND Vx, Vy"}},
    {{0x8, NA, NA, 0x3}, Opcode {"XOR Vx, Vy"}},
    {{0x8, NA, NA, 0x4}, Opcode {"ADD Vx, Vy"}},
    {{0x8, NA, NA, 0x5}, Opcode {"SUB Vx, Vy"}},
    {{0x8, NA, NA, 0x6}, Opcode {"SHR Vx {, Vy}"}},
    {{0x8, NA, NA, 0x7}, Opcode {"SUBN Vx, Vy"}},
    {{0x8, NA, NA, 0xE}, Opcode {"SHL Vx {, Vy}"}},
    {{0x9, NA, NA, 0x0}, Opcode {"SNE Vx, Vy"}},
    {{0xA, NA, NA, NA}, Opcode {"LD I, addr"}},
    {{0xB, NA, NA, NA}, Opcode {"JP V0, addr"}},
    {{0xC, NA, NA, NA}, Opcode {"RND Vx, byte"}},
    {{0xD, NA, NA, NA}, Opcode {"DRW Vx, Vy, nibble"}},
    {{0xE, NA, 0x9, 0xE}, Opcode {"SKP Vx"}},
    {{0xE, NA, 0xA, 0x1}, Opcode {"SKNP Vx"}},
    {{0xF, NA, 0x0, 0x7}, Opcode {"LD Vx, DT"}},
    {{0xF, NA, 0x0, 0xA}, Opcode {"LD Vx, K"}},
    {{0xF, NA, 0x1, 0x5}, Opcode {"LD DT, Vx"}},
    {{0xF, NA, 0x1, 0x8}, Opcode {"LD ST, Vx"}},
    {{0xF, NA, 0x1, 0xE}, Opcode {"ADD I, Vx"}},
    {{0xF, NA, 0x2, 0x9}, Opcode {"LD F, Vx"}},
    {{0xF, NA, 0x3, 0x3}, Opcode {"LD B, Vx"}},
    {{0xF, NA, 0x5, 0x5}, Opcode {"LD [I], Vx"}},
    {{0xF, NA, 0x6, 0x5}, Opcode {"LD Vx, [I]"}},
};

static const size_t GFX_W = 64;
static const size_t GFX_H = 32;
static const size_t GFX_SIZE = GFX_W * GFX_H;

static std::random_device dev;
static std::mt19937 rng(dev());
static std::uniform_int_distribution<uint8_t> dist(1,255);

class Chip8
{
//private:
public:
    uint8_t memory[4096] = {0};
    uint8_t V[16] = {0};
    uint16_t I = 0;

    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;

    uint16_t pc = 0x200;
    uint8_t sp = 0;
    uint16_t stack[16] = {0};

    bool keys[16] = {0};

public:
    std::bitset<GFX_SIZE> gfx = {false};
    std::string loaded_rom;

    Chip8();
    void load(std::string filepath);
    void tick();
    uint16_t getIns();
    uint8_t getV(size_t);
    uint16_t getPC();
    void reset();
    std::optional<Opcode> getOpcode(uint16_t ins);
};

#endif