# Chip8-Emulator
Everyone's first emulator.

Not perfect, tried to add in some more modern C++ features.
Tried to make super simplified pattern matching similar to how I would use Rust match for this. There are some proposals for C++ native pattern matching that have been kicking around for a while (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1371r3.pdf) When something like this gets approved in 2030, I can't wait to use it in 2050.

### Build
Tested only on Linux

```make```

#### Keys
Chip8 Input:
1 2 3 4
Q W E R
A S D F
Z X C V

Space: Step tick
Left Control: Run while key is down
Enter: Run program
F5: Reset

#### Roms
Included just a few random ones stolen from places

#### Todo
- Better fonts
- Rom Picker
- Fix duplicate lookup table matching code
- Add compatibility for Super Chip-48