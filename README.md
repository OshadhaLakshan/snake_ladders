# 🐍🎲 Snake and Ladders – SDL2 Game

A classic Snake and Ladders board game built in **C** using **SDL2**, **SDL_image**, and **SDL_ttf**. This project features a fully playable 10x10 board, randomly placed snakes and ladders, token selection, dice rolling, and win condition.

---

## 🚀 Features

- 🎮 10x10 serpentine Snake and Ladders board
- 🐍 Random snakes and ladders on each run
- 🎲 Dice rolling with visual feedback
- 🧍‍♂️ 4 colored tokens (Red, Green, Blue, Yellow)
- 🟢 Selected token is highlighted
- ↩️ Tokens move with `RETURN` key
- 🏁 Game ends with a winning message when a token reaches tile 100

---

## 📸 Screenshots

> ![image](https://github.com/user-attachments/assets/2454c9d4-18cf-48b5-93d7-b0403e96ba98)
> ![Screenshot 2025-06-22 194419](https://github.com/user-attachments/assets/2f6804ce-92bf-4097-8d22-65bd55889904)
> ![image](https://github.com/user-attachments/assets/a2b20ed4-dc0b-44c8-9488-3f7b009f8bb5)


*Sample PlayThrough of the Game*
---

## 🛠️ Dependencies

- [SDL2](https://www.libsdl.org/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)

Make sure you have the development libraries installed.

---

## 🧑‍💻 Build Instructions

### 🔧 For MinGW/MSYS2 (Windows)

```bash
gcc main.c icon.res -o snake_ladders -Iinclude -Llib -lSDL2 -lSDL2_image -lSDL2_ttf
./snake_ladders
```

## 🧰 Folder Structure

```vbnet
project/
├── assets/
│   ├── snake1.png
│   ├── snake2.png
│   ├── snake3.png
│   ├── snake4.png
│   ├── ladder1.png
│   └── timesbd.ttf
├── main.c
├── include/
│   └── SDL headers (optional)
├── lib/
│   └── SDL2 libraries (optional)
└── README.md
```

## 🎮 Controls

| Key    | Action                   |
|--------|--------------------------|
| TAB    | Cycle through tokens     |
| RETURN | Roll dice and move token |
| ESC    | Exit game (add manually) |

## 📄 License

MIT License © 2025 Oshadha Lakshan

## 🙌 Credits

- SDL2 Library by SDL Foundation  
- Game logic and design by Oshadha Lakshan
