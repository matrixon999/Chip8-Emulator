#include "chip8.h"

Chip8::Chip8() {
    std::copy(FONT_DATA, FONT_DATA+80, memory);
}

void Chip8::load(std::string filepath)
{
    loaded_rom = filepath;
    std::streampos size;
    std::ifstream romfile(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    if(romfile.is_open()) {
        size = romfile.tellg();
        romfile.seekg(0, std::ios::beg);
        romfile.read((char*)(&memory[0x200]), size);
        romfile.close();
    }
}

void Chip8::tick()
{
    // get instruction
    uint16_t ins = getIns();
    // pull apart to component nibbles
    uint16_t nnn = ins & 0x0FFF;
    uint8_t a = (ins & 0xF000) >> 12;
    uint8_t x = (ins & 0x0F00) >> 8;
    uint8_t y = (ins & 0x00F0) >> 4;
    uint8_t n = (ins & 0x000F);
    uint8_t kk = ins & 0x00FF;

    auto opcode = getOpcode(ins);
    if(opcode) {
        const std::string& mnemonic = opcode->mnemonic;
        fmt::print("{:#6x} {}\n", ins, mnemonic);
    } else {
        fmt::print(" >> {} <<  {:X} {:X} {:X} {:X}\n", "ERROR", a, x, y, n);
    }
    
    if(util::match(ins, 0x0, 0x0, 0xE, 0x0)) {    // 00E0 CLS
        gfx.reset();
    } else if(util::match(ins, 0x0, 0x0, 0xE, 0xE)) {    // 00EE RET
        pc = stack[--sp];
    } else if(util::match(ins, 0x1, NA, NA, NA)) {    // 1NNN JMP
        pc = nnn;
        pc -= 2;
    } else if(util::match(ins, 0x2, NA, NA, NA)) {    // 2NNN CALL
        stack[sp] = pc;
        sp++;
        pc = nnn;
        pc-=2;
    } else if(util::match(ins, 0x3, NA, NA, NA)) {    // 3XNN SE Vx, byte
        if(V[x] == kk) pc += 2;
    } else if(util::match(ins, 0x4, NA, NA, NA)) {    // 4XNN SNE Vx, byte
        if(V[x] != kk) pc += 2;
    } else if(util::match(ins, 0x5, NA, NA, 0x0)) {    // 5XY0 SE Vx, Vy
        if(V[x] == V[y]) pc += 2;
    } else if(util::match(ins, 0x6, NA, NA, NA)) {    // 6XNN LD Vx, byte
        V[x] = kk;
    } else if(util::match(ins, 0x7, NA, NA, NA)) {    // 7XNN ADD Vx, byte
        V[x] += kk;
    } else if(util::match(ins, 0x8, NA, NA, 0x0)) {    // 8XY0 LD Vx, Vy
        V[x] = V[y];
    } else if(util::match(ins, 0x8, NA, NA, 0x1)) {    // 8XY1 OR Vx, Vy
        V[x] |= V[y];
    } else if(util::match(ins, 0x8, NA, NA, 0x2)) {    // 8XY2 AND Vx, Vy
        V[x] &= V[y];
    } else if(util::match(ins, 0x8, NA, NA, 0x3)) {    // 8XY3 XOR Vx, Vyt
        V[x] ^= V[y];
    } else if(util::match(ins, 0x8, NA, NA, 0x4)) {    // 8XY4 ADD Vx, Vy
        uint16_t tmp = (uint16_t)V[x] + (uint16_t)V[y];
        V[0xF] = (tmp > 255);
        V[x] = (uint8_t)tmp;
    } else if(util::match(ins, 0x8, NA, NA, 0x5)) {    // 8XY5 SUB Vx, Vy
        V[0xF] = (V[x] > V[y]);
        V[x] -= V[y];
    } else if(util::match(ins, 0x8, NA, NA, 0x6)) {    // 8XY6 SHR Vx {, Vy}
        V[0xF] = ((V[x] & 0x1) > 0);
        V[x] >>= 1;
    } else if(util::match(ins, 0x8, NA, NA, 0x7)) {    // 8XY7 SUBN Vx, Vy
        V[0xF] = (V[y] > V[x]);
        V[x] = V[y] - V[x];
    } else if(util::match(ins, 0x8, NA, NA, 0xE)) {    // 8XYE SHL Vx {, Vy}
        V[0xF] = ((V[x] & 0b10000000) > 0);
        V[x] <<= 1;
    } else if(util::match(ins, 0x9, NA, NA, 0x0)) {    // 9XY0 SNE Vx, Vy
        if(V[x] != V[y]) pc += 2;
    } else if(util::match(ins, 0xA, NA, NA, NA)) {    // ANNN LD I, addr
        I = nnn;
    } else if(util::match(ins, 0xB, NA, NA, NA)) {    // BNNN JP V0, addr
        pc = nnn + V[0];
        pc -= 2;
    } else if(util::match(ins, 0xC, NA, NA, NA)) {    // CXNN RND Vx, byte
        V[x] = dist(rng) & kk;
    } else if(util::match(ins, 0xD, NA, NA, NA)) {    // DXYN DRW Vx, Vy, nibble
        V[0xF] = 0;
        for(int row = 0; row < n; row++) {
            std::bitset<8> pixelbits(memory[I + row]);

            for(int col = 0; col < 8; col++) {
                size_t xpos = (V[x] + col) % GFX_W;
                size_t ypos = (V[y] + row) % GFX_H;
                size_t pos = ypos * GFX_W + xpos;
                if(pixelbits[7-col] && gfx[pos]) V[0xF] = 1;
                gfx[pos] = gfx[pos] ^ pixelbits[7-col];
            }
        }

        /*for(int y = 0; y < n; y++) {
            uint8_t row = memory[I + y];
            size_t pos = V[y] * GFX_W + V[x];
            V[0xF] = V[0xF] || (gfx[pos] ^ row) > 0;
            std::bitset<8> rowbits(row);

            for(int i = 0; i < 8; i++) {
                gfx[pos + i] = gfx[pos + i] ^ rowbits[i];
            }
        }*/
    } else if(util::match(ins, 0xE, NA, 0x9, 0xE)) {    // EX9E SKP Vx
        if(keys[V[x]]) pc += 2;
    } else if(util::match(ins, 0xE, NA, 0xA, 0x1)) {    // EXA1 SKNP Vx
        if(!keys[V[x]]) pc += 2;
    } else if(util::match(ins, 0xF, NA, 0x0, 0x7)) {    // FX07 LD Vx, DT
        V[x] = delay_timer;
    } else if(util::match(ins, 0xF, NA, 0x0, 0xA)) {    // FX0A LD Vx, K
        if(std::none_of(keys, keys+16, [](bool down) { return down; })) {
            return;
        }
    } else if(util::match(ins, 0xF, NA, 0x1, 0x5)) {    // FX15 LD DT, Vx
        delay_timer = V[x];
    } else if(util::match(ins, 0xF, NA, 0x1, 0x8)) {    // FX18 LD ST, Vx
        sound_timer = V[x];
    } else if(util::match(ins, 0xF, NA, 0x1, 0xE)) {    // FX1E ADD I, Vx
        I += V[x];
    } else if(util::match(ins, 0xF, NA, 0x2, 0x9)) {    // FX29 LD F, Vx
        //util::error("Unimplemented"); exit(-1);
        I = memory[V[x]*5]; // TODO DOUBLE CHECK
    } else if(util::match(ins, 0xF, NA, 0x3, 0x3)) {    // FX33 LD B, Vx
        memory[I] = V[x] / 100;
        memory[I+1] = (V[x] / 10) % 10;
        memory[I+2] = V[x] % 10;
    } else if(util::match(ins, 0xF, NA, 0x5, 0x5)) {    // FX55 LD [I] Vx
        std::copy(V, V + x + 1, memory + I);
    } else if(util::match(ins, 0xF, NA, 0x6, 0x5))  {    // FX65 LD Vx [I]
        std::copy(memory + I, memory + I + x + 1, V);
    } else {
        util::error("I have no idea what this is");
    }

    if(delay_timer > 0) delay_timer -= 1;
    if(sound_timer > 0) sound_timer -= 1;

    pc += 2;
}

uint16_t Chip8::getIns()
{
    return ((uint16_t)memory[pc] << 8) | (uint16_t)memory[pc + 1];
}

uint8_t Chip8::getV(size_t i)
{
    return V[i];
}

uint16_t Chip8::getPC()
{
    return pc;
}

void Chip8::reset()
{
    gfx.reset();

    std::fill(memory, memory+4096, 0);
    std::copy(FONT_DATA, FONT_DATA+80, memory);
    load(loaded_rom);

    std::fill(V, V+16, 0);
    std::fill(stack, stack+16, 0);

    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    pc = 0x200;
    sp = 0;
}

std::optional<Opcode> Chip8::getOpcode(uint16_t ins) {
    uint16_t a = (ins & 0xF000) >> 12;
    uint16_t x = (ins & 0x0F00) >> 8;
    uint16_t y = (ins & 0x00F0) >> 4;
    uint16_t n = (ins & 0x000F);

    std::array<uint16_t, 4> key = {a,x,y,n};

    for (const OpcodeEntry& entry : OPCODES) {
        bool match = true;
        for(size_t i = 0; i < key.size(); ++i) {
            if(entry.key[i] != key[i] && entry.key[i] != NA) {
                match = false;
                break;
            }
        }
        if(match) {
            return entry.opcode;
        }
    }
    return std::nullopt;
}
