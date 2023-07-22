#include <iostream>
#include "raylib.h"
#include "chip8.h"
#include <fmt/core.h>
#include <string>

int main() {

    Chip8 chip8;
    //chip8.load("roms/3-corax+.ch8");
    //chip8.load("roms/Space Invaders [David Winter].ch8");
    chip8.load("roms/MISSILE");

    const int tileSize = 10;
    const int screenWidth = GFX_W*tileSize;
    const int screenHeight = GFX_H*tileSize;

    const int keys[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_C,
                        KEY_Q, KEY_W, KEY_E, KEY_R,
                        KEY_A, KEY_S, KEY_D, KEY_F,
                        KEY_Z, KEY_X, KEY_C, KEY_V};

    const int debugMenuWidth = 225;

    InitWindow(screenWidth + debugMenuWidth, screenHeight, "Chip-8 Emulator");

    SetTargetFPS(60);

    Font font = LoadFontEx("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16, 0, 250);
    //Font font = GetFontDefault();

    bool run = false;
    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_LEFT_CONTROL) || run) {
            chip8.tick();
        }
        if(IsKeyPressed(KEY_F5)) {
            chip8.reset();
        }
        if(IsKeyPressed(KEY_ENTER)) {
            run = !run;
        }

        for(int i = 0; i < 16; i++) {
            if(IsKeyPressed(keys[i])) chip8.keys[i] = true;
            if(IsKeyReleased(keys[i])) chip8.keys[i] = false;
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);
            for(int i = 0; i < GFX_SIZE; i++) {
                if(chip8.gfx[i]) {
                    int x = i % GFX_W;
                    int y = i / GFX_W;
                    DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, RAYWHITE);
                }
            }

            DrawRectangle(screenWidth, 0, debugMenuWidth, screenHeight, LIGHTGRAY);

            uint16_t ins = chip8.getIns();
            std::string out = fmt::format("Ins = {:#06x}", ins);
            DrawTextEx(font, out.c_str(), {screenWidth + 10, 10}, 16, 1, BLACK);

            out = fmt::format("PC = {:#06x}", chip8.getPC());
            DrawTextEx(font, out.c_str(), {screenWidth + 10, 50}, 16, 1, BLACK);
            
            out = fmt::format("I = {:#06x}", chip8.I);
            DrawTextEx(font, out.c_str(), {screenWidth + 10, 70}, 16, 1, BLACK);

            out = fmt::format("SP = {:#06x}", chip8.sp);
            DrawTextEx(font, out.c_str(), {screenWidth + 10, 90}, 16, 1, BLACK);

            for(int i = 0; i < 16; i++) {
                out = fmt::format("V{:X} = {:#04x}", i, chip8.getV(i));
                DrawTextEx(font, out.c_str(), {(float)screenWidth + 10 + ((debugMenuWidth / 2)*(i>7)), 150.0f + (i-(8*(i>7))) * 15}, 16, 1, BLACK);
            }

            auto opcode = chip8.getOpcode(ins);
            if(opcode) {
                out = opcode->mnemonic;
            } else {
                out = "UNKOWN";
            }
            DrawTextEx(font, out.c_str(), {screenWidth + 10, 30}, 16, 1, BLACK);


        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}