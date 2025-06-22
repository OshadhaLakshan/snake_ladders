#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <time.h>

// Game configuration constants
#define NUM_SNAKES 4
#define NUM_LADDERS 3
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define MARGIN 20
#define BOARD_SIZE 10
#define TILE_SIZE ((SCREEN_WIDTH - 2 * MARGIN) / BOARD_SIZE)

typedef struct {
    int position;   // Current tile (1–100)
    SDL_Color color;
} Token;

#define NUM_TOKENS 4
Token tokens[NUM_TOKENS] = {
    {1, {255, 0, 0}},    // Red
    {1, {0, 255, 0}},    // Green
    {1, {0, 0, 255}},    // Blue
    {1, {255, 255, 0}},  // Yellow
};

int selectedToken = 0;
int diceValue = 1;
bool diceRolled = false;
bool gameWon = false;

// Forward declarations
void draw_board(SDL_Renderer* renderer, TTF_Font* font);
void generate_snakes_and_ladders(int snakes[NUM_SNAKES][2], int ladders[NUM_LADDERS][2]);
void place_snakes_and_ladders(SDL_Renderer* renderer, SDL_Texture* snakeTexArray[4], SDL_Texture* ladderTex,
                              int snakes[NUM_SNAKES][2], int ladders[NUM_LADDERS][2]);
void draw_arrow(SDL_Renderer* renderer, SDL_Texture* tex, int from, int to);
void draw_tiled_arrow(SDL_Renderer* renderer, SDL_Texture* tex, int from, int to);
bool is_conflicting(int start, int end, int existing[][2], int count);
void draw_tokens(SDL_Renderer* renderer, Token tokens[], int selected);

int main() {
    srand((unsigned int)time(NULL));

    int snakes[NUM_SNAKES][2];
    int ladders[NUM_LADDERS][2];
    generate_snakes_and_ladders(snakes, ladders);

    // Debug print snakes and ladders
    printf("Snakes:\n");
    for (int i = 0; i < NUM_SNAKES; i++) {
        printf("  Snake %d: Head %d -> Tail %d\n", i+1, snakes[i][0], snakes[i][1]);
    }
    printf("Ladders:\n");
    for (int i = 0; i < NUM_LADDERS; i++) {
        printf("  Ladder %d: Bottom %d -> Top %d\n", i+1, ladders[i][0], ladders[i][1]);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) == 0 || TTF_Init() < 0) {
        fprintf(stderr, "SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake and Ladders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit(); return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit(); return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/timesbd.ttf", 14);
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Texture* snakeTextures[4];
    char path[64];

    for (int i = 0; i < 4; ++i) {
        snprintf(path, sizeof(path), "assets/snake%d.png", i + 1);
        snakeTextures[i] = IMG_LoadTexture(renderer, path);
        if (!snakeTextures[i]) {
            fprintf(stderr, "Failed to load %s\n", path);
            return 1;
        }
    }

    SDL_Texture* ladderTexture = IMG_LoadTexture(renderer, "assets/ladder1.png");

    if (!snakeTextures[4] || !ladderTexture) {
        fprintf(stderr, "Failed to load textures\n");
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {

                    case SDLK_RETURN:
                        if (!gameWon) {
                            diceValue = (rand() % 6) + 1;
                            diceRolled = true;

                            Token *token = &tokens[selectedToken];
                            int newPos = token->position + diceValue;
                            if (newPos <= 100) {
                                token->position = newPos;

                                // Check for snake
                                for (int i = 0; i < NUM_SNAKES; ++i) {
                                    if (token->position == snakes[i][0]) {
                                        token->position = snakes[i][1];
                                        break;
                                    }
                                }

                                // Check for ladder
                                for (int i = 0; i < NUM_LADDERS; ++i) {
                                    if (token->position == ladders[i][0]) {
                                        token->position = ladders[i][1];
                                        break;
                                    }
                                }
                            }

                            if (token->position == 100) {
                                gameWon = true;
                                break;
                            }
                            selectedToken = (selectedToken + 1) % NUM_TOKENS;
                            break;
                        }

                    case SDLK_ESCAPE:
                        running = false;
                        break;
                }
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        draw_board(renderer, font);
        place_snakes_and_ladders(renderer, snakeTextures, ladderTexture, snakes, ladders);
        draw_tokens(renderer, tokens, selectedToken);

        // Draw dice value
        char diceStr[16];
        snprintf(diceStr, sizeof(diceStr), "Dice: %d", diceValue);
        SDL_Surface* diceSurf = TTF_RenderText_Solid(font, diceStr, (SDL_Color){0, 0, 0});
        SDL_Texture* diceTex = SDL_CreateTextureFromSurface(renderer, diceSurf);
        SDL_Rect diceRect = { SCREEN_WIDTH / 2 - diceSurf->w / 2, SCREEN_HEIGHT - MARGIN, diceSurf->w, diceSurf->h };
        SDL_RenderCopy(renderer, diceTex, NULL, &diceRect);
        SDL_FreeSurface(diceSurf);
        SDL_DestroyTexture(diceTex);

        // Show winner
        if (gameWon) {
            const char* winnerNames[] = { "Red", "Green", "Blue", "Yellow" };
            char winText[64];
            snprintf(winText, sizeof(winText), "%s Wins!", winnerNames[selectedToken]);
            SDL_Surface* winSurf = TTF_RenderText_Solid(font, winText, (SDL_Color){0, 0, 0});
            SDL_Texture* winTex = SDL_CreateTextureFromSurface(renderer, winSurf);
            SDL_Rect winRect = { SCREEN_WIDTH / 2 - winSurf->w / 2, 3, winSurf->w, winSurf->h };
            SDL_RenderCopy(renderer, winTex, NULL, &winRect);
            SDL_FreeSurface(winSurf);
            SDL_DestroyTexture(winTex);
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(snakeTextures[i]);
    }
    SDL_DestroyTexture(ladderTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit(); IMG_Quit(); SDL_Quit();

    return 0;
}

void draw_board(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int x = MARGIN + col * TILE_SIZE;
            int y = MARGIN + (BOARD_SIZE - 1 - row) * TILE_SIZE;
            SDL_Rect tileRect = { x, y, TILE_SIZE, TILE_SIZE };

            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &tileRect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &tileRect);

            // Compute tile number in serpentine order starting from bottom-left
            int tileNum;
            if (row % 2 == 0) {
                tileNum = row * BOARD_SIZE + col + 1;
            } else {
                tileNum = row * BOARD_SIZE + (BOARD_SIZE - col);
            }

            char numText[4];
            snprintf(numText, sizeof(numText), "%d", tileNum);
            SDL_Color textColor = { 0, 0, 0 };
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, numText, textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { x + 4, y + 2, textSurface->w, textSurface->h };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }
}

void draw_tokens(SDL_Renderer* renderer, Token tokens[], int selected) {
    for (int i = 0; i < NUM_TOKENS; ++i) {
        int pos = tokens[i].position;
        int row = (pos - 1) / BOARD_SIZE;
        int col = (pos - 1) % BOARD_SIZE;
        if (row % 2 != 0) col = BOARD_SIZE - 1 - col;

        int x = MARGIN + col * TILE_SIZE + TILE_SIZE / 2;
        int y = MARGIN + (BOARD_SIZE - 1 - row) * TILE_SIZE + TILE_SIZE / 2;

        SDL_SetRenderDrawColor(renderer,
            tokens[i].color.r,
            tokens[i].color.g,
            tokens[i].color.b,
            255);
        SDL_Rect tokenRect = { x - 10, y - 10, 20, 20 };
        SDL_RenderFillRect(renderer, &tokenRect);

        if (i == selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Highlight
            SDL_Rect border = { x - 12, y - 12, 24, 24 };
            SDL_RenderDrawRect(renderer, &border);
        }
    }
}


bool is_conflicting(int start, int end, int existing[][2], int count) {
    for (int i = 0; i < count; ++i) {
        if (abs(start - existing[i][0]) < 5 || abs(start - existing[i][1]) < 5 ||
            abs(end - existing[i][0]) < 5 || abs(end - existing[i][1]) < 5)
            return true;
    }
    return false;
}

void generate_snakes_and_ladders(int snakes[NUM_SNAKES][2], int ladders[NUM_LADDERS][2]) {
    bool used[101] = { false };

    for (int i = 0; i < NUM_SNAKES; i++) {
        int head, tail, attempts = 0;
        do {
            head = rand() % 50 + 41;  // tile 41-90
            tail = rand() % (head - 15) + 10;
            attempts++;
        } while ((used[head] || used[tail] || head <= tail ||
                 is_conflicting(head, tail, snakes, i) ||
                 is_conflicting(head, tail, ladders, NUM_LADDERS)) && attempts < 1000);

        snakes[i][0] = head;
        snakes[i][1] = tail;
        used[head] = used[tail] = true;
    }

    for (int i = 0; i < NUM_LADDERS; i++) {
        int bottom, top, attempts = 0;
        do {
            bottom = rand() % 50 + 5;  // tile 5-54
            top = rand() % (100 - bottom - 10) + (bottom + 10);
            attempts++;
        } while ((used[top] || used[bottom] || top <= bottom ||
                 is_conflicting(top, bottom, ladders, i) ||
                 is_conflicting(top, bottom, snakes, NUM_SNAKES)) && attempts < 1000);

        ladders[i][0] = bottom;
        ladders[i][1] = top;
        used[bottom] = used[top] = true;
    }
}

void place_snakes_and_ladders(SDL_Renderer* renderer, SDL_Texture* snakeTexArray[4], SDL_Texture* ladderTex,
                              int snakes[NUM_SNAKES][2], int ladders[NUM_LADDERS][2]) {
    for (int i = 0; i < NUM_SNAKES; ++i)
        draw_arrow(renderer, snakeTexArray[i % 4], snakes[i][0], snakes[i][1]);

    for (int i = 0; i < NUM_LADDERS; ++i)
        draw_tiled_arrow(renderer, ladderTex, ladders[i][0], ladders[i][1]);
}

void draw_arrow(SDL_Renderer* renderer, SDL_Texture* tex, int from, int to) {
    // Convert tiles to board (row, col) with serpentine logic
    int fromRow = (from - 1) / BOARD_SIZE;
    int fromCol = (from - 1) % BOARD_SIZE;
    if (fromRow % 2 == 1) fromCol = BOARD_SIZE - 1 - fromCol;

    int toRow = (to - 1) / BOARD_SIZE;
    int toCol = (to - 1) % BOARD_SIZE;
    if (toRow % 2 == 1) toCol = BOARD_SIZE - 1 - toCol;

    // Convert to screen coordinates
    int fromX = MARGIN + fromCol * TILE_SIZE + TILE_SIZE / 2;
    int fromY = MARGIN + (BOARD_SIZE - fromRow - 1) * TILE_SIZE + TILE_SIZE / 2;
    int toX   = MARGIN + toCol * TILE_SIZE + TILE_SIZE / 2;
    int toY   = MARGIN + (BOARD_SIZE - toRow - 1) * TILE_SIZE + TILE_SIZE / 2;

    float dx = toX - fromX;
    float dy = toY - fromY;
    float baseAngle = atan2f(dy, dx) * 180.0f / M_PI;
    float angle = baseAngle - 90.0f;  // Rotate 90° anti-clockwise
    float length = sqrtf(dx * dx + dy * dy);

    int texW, texH;
    SDL_QueryTexture(tex, NULL, NULL, &texW, &texH);

    // Scale while keeping aspect ratio (height = length, width scaled proportionally)
    float scale = length / texH;
    int drawW = texW * scale;
    int drawH = texH * scale;

    SDL_Rect dest = {
        (fromX + toX) / 2 - drawW / 2,
        (fromY + toY) / 2 - drawH / 2,
        drawW,
        drawH
    };

    // Rotate snake from head (from) to tail (to)
    SDL_RenderCopyEx(renderer, tex, NULL, &dest, angle, NULL, SDL_FLIP_VERTICAL);
}

void draw_tiled_arrow(SDL_Renderer* renderer, SDL_Texture* tex, int from, int to) {
    int fromRow = (from - 1) / BOARD_SIZE;
    int fromCol = (from - 1) % BOARD_SIZE;
    if (fromRow % 2 != 0) fromCol = BOARD_SIZE - 1 - fromCol;

    int toRow = (to - 1) / BOARD_SIZE;
    int toCol = (to - 1) % BOARD_SIZE;
    if (toRow % 2 != 0) toCol = BOARD_SIZE - 1 - toCol;

    int fromX = MARGIN + fromCol * TILE_SIZE + TILE_SIZE / 2;
    int fromY = MARGIN + (BOARD_SIZE - fromRow - 1) * TILE_SIZE + TILE_SIZE / 2;
    int toX = MARGIN + toCol * TILE_SIZE + TILE_SIZE / 2;
    int toY = MARGIN + (BOARD_SIZE - toRow - 1) * TILE_SIZE + TILE_SIZE / 2;

    float dx = toX - fromX, dy = toY - fromY;
    float angle = atan2f(dy, dx) * 180.0f / M_PI;
    float length = sqrtf(dx * dx + dy * dy);
    if (length < 1.0f) return;

    int imgW, imgH;
    SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH);

    float scale = (float)TILE_SIZE / imgW; // Since we are rotating 90°, width maps to height
    int drawW = imgW * scale;
    int drawH = imgH * scale;

    int tiles = (int)(length / drawH);
    float ux = dx / length;
    float uy = dy / length;

    for (int i = 0; i <= tiles; ++i) {
        float fx = fromX + ux * drawH * i;
        float fy = fromY + uy * drawH * i;

        SDL_Rect destRect = {
            (int)(fx - drawW / 2),
            (int)(fy - drawH / 2),
            drawW,
            drawH
        };

        SDL_RenderCopyEx(renderer, tex, NULL, &destRect, angle + 90.0f, NULL, SDL_FLIP_NONE);
    }
}
