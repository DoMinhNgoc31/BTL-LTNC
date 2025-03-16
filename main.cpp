#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 10;
const int CELL_SIZE = SCREEN_HEIGHT / GRID_SIZE;

struct Player {
    int position;
    SDL_Color color;
};

int lastRoll = 1;
bool gameWon = false;
std::string winner = "";

// Load hình ảnh xúc xắc
SDL_Texture* diceTextures[6] = {nullptr};

int rollDice() {
    lastRoll = rand() % 6 + 1;
    return lastRoll;
}

int getRow(int pos) {
    return GRID_SIZE - 1 - (pos - 1) / GRID_SIZE;
}

int getCol(int pos) {
    int row = (pos - 1) / GRID_SIZE;
    if (row % 2 == 0) {
        return (pos - 1) % GRID_SIZE;
    } else {
        return GRID_SIZE - 1 - (pos - 1) % GRID_SIZE;
    }
}

void movePlayer(Player &player, int roll, std::string playerName) {
    if (player.position + roll <= 100) {
        player.position += roll;
    }

    if (player.position == 100) {
        gameWon = true;
        winner = playerName;
    }

    int snakes[] = {16, 47, 49, 56, 62, 64, 87, 93, 95, 98};
    int snakeEnds[] = {6, 26, 11, 53, 19, 60, 24, 73, 75, 78};
    int ladders[] = {1, 4, 9, 21, 28, 36, 51, 71, 80};
    int ladderEnds[] = {38, 14, 31, 42, 84, 44, 67, 91, 97};

    for (int i = 0; i < 10; i++) {
        if (player.position == snakes[i]) {
            player.position = snakeEnds[i];
        }
    }
    for (int i = 0; i < 9; i++) {
        if (player.position == ladders[i]) {
            player.position = ladderEnds[i];
        }
    }
}

void drawBoard(SDL_Renderer *renderer) {
    for (int i = 0; i <= GRID_SIZE; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_HEIGHT, i * CELL_SIZE);
    }
}

void drawSnakesAndLadders(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    int snakes[] = {16, 47, 49, 56, 62, 64, 87, 93, 95, 98};
    int snakeEnds[] = {6, 26, 11, 53, 19, 60, 24, 73, 75, 78};
    int ladders[] = {1, 4, 9, 21, 28, 36, 51, 71, 80};
    int ladderEnds[] = {38, 14, 31, 42, 84, 44, 67, 91, 97};

    for (int i = 0; i < 10; i++) {
        SDL_RenderDrawLine(renderer, getCol(snakes[i]) * CELL_SIZE + CELL_SIZE / 2, getRow(snakes[i]) * CELL_SIZE + CELL_SIZE / 2,
                           getCol(snakeEnds[i]) * CELL_SIZE + CELL_SIZE / 2, getRow(snakeEnds[i]) * CELL_SIZE + CELL_SIZE / 2);
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < 9; i++) {
        SDL_RenderDrawLine(renderer, getCol(ladders[i]) * CELL_SIZE + CELL_SIZE / 2, getRow(ladders[i]) * CELL_SIZE + CELL_SIZE / 2,
                           getCol(ladderEnds[i]) * CELL_SIZE + CELL_SIZE / 2, getRow(ladderEnds[i]) * CELL_SIZE + CELL_SIZE / 2);
    }
}

void drawPlayer(SDL_Renderer *renderer, Player player) {
    SDL_Rect rect = {getCol(player.position) * CELL_SIZE, getRow(player.position) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(renderer, player.color.r, player.color.g, player.color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void drawDice(SDL_Renderer *renderer) {
    SDL_Rect diceRect = {SCREEN_HEIGHT + 20, 50, 80, 80};
    if (diceTextures[lastRoll - 1]) {
        SDL_RenderCopy(renderer, diceTextures[lastRoll - 1], NULL, &diceRect);
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Snakes and Ladders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(0));
    Player player1 = {1, {255, 0, 0}};
    Player player2 = {1, {0, 0, 255}};

    // Load hình ảnh xúc xắc
    for (int i = 0; i < 6; i++) {
        std::string filePath = "images/dice_" + std::to_string(i + 1) + ".png";
        diceTextures[i] = IMG_LoadTexture(renderer, filePath.c_str());
        if (!diceTextures[i]) {
            std::cerr << "Error loading " << filePath << ": " << IMG_GetError() << std::endl;
        }
    }

    bool quit = false;
    SDL_Event e;
    int turn = 1;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (!gameWon && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                int roll = rollDice();
                movePlayer(turn == 1 ? player1 : player2, roll, turn == 1 ? "Player 1" : "Player 2");
                if (!gameWon && roll != 6) turn = 3 - turn;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBoard(renderer);
        drawSnakesAndLadders(renderer);
        drawDice(renderer);
        drawPlayer(renderer, player1);
        drawPlayer(renderer, player2);
        SDL_RenderPresent(renderer);
    }

    for (auto &tex : diceTextures) SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
