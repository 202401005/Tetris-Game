# Tetris Game (C++ Console)

This is a basic Tetris game implemented in C++ for the Windows console.

## Features

- Standard Tetris shapes (I, O, T, S, Z, J, L)
- Keyboard controls for movement and rotation
- Line clearing and score tracking
- Level increases based on score
- Game over detection

## Controls

- Left Arrow: Move left  
- Right Arrow: Move right  
- Up Arrow: Rotate  
- Down Arrow: Soft drop  
- Spacebar: Hard drop  
- Esc: Exit game

## How to Compile

This game uses Windows-specific headers (`windows.h`, `conio.h`), so it must be compiled on Windows.

```bash
g++ -o tetris tetris.cpp
