#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;
const int GRID_SIZE = 8;
const int CELL_SIZE = SCREEN_HEIGHT / GRID_SIZE;

struct Player {
    int position;
};

int lastRoll = 1;
bool gameWon = false;
std::string winner = "";

SDL_Texture* diceTextures[6] = {nullptr};
SDL_Texture* boardTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* playerTextures[2] = {nullptr};
TTF_Font* font = nullptr;

int rollDice() {
    lastRoll = rand() % 6 + 1;
    return lastRoll;
}

int getRow(int pos) {
    return (GRID_SIZE - 1) - ((pos - 1) / GRID_SIZE);
}

int getCol(int pos) {
    int row = (pos - 1) / GRID_SIZE;
    if (row % 2 == 0) {
        return (pos - 1) % GRID_SIZE;
    } else {
        return GRID_SIZE - 1 - (pos - 1) % GRID_SIZE;
    }
}

void drawBackground(SDL_Renderer *renderer) {
    if (backgroundTexture) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
    }
}

void drawBoard(SDL_Renderer *renderer) {
    if (boardTexture) {
        SDL_Rect boardRect = {150, 0, SCREEN_HEIGHT, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, boardTexture, NULL, &boardRect);
    }
}

void drawPlayer(SDL_Renderer *renderer, Player player, int index) {
    if (player.position > 0 && playerTextures[index]) {
        SDL_Rect rect = {
            150 + getCol(player.position) * CELL_SIZE + CELL_SIZE / 4 - CELL_SIZE / 4,
            getRow(player.position) * CELL_SIZE + CELL_SIZE / 4 - CELL_SIZE / 4,
            CELL_SIZE,
            CELL_SIZE
        };
        SDL_RenderCopy(renderer, playerTextures[index], NULL, &rect);
    }
}

void drawDice(SDL_Renderer *renderer) {
    SDL_Rect diceRect = {20, 50, 80, 80};
    if (diceTextures[lastRoll - 1]) {
        SDL_RenderCopy(renderer, diceTextures[lastRoll - 1], NULL, &diceRect);
    }
}

void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void checkSnakesAndLadders(Player& player) {
    int snakes[] = {35,39,44,47,57,58,60};
    int snakeEnds[] = {3,4,24,34,40,45,46};
    int ladders[] = {5,6,8,15,19,23,28,42,43,50};
    int ladderEnds[] = {21,11,24,31,36,40,38,59,54,63};

    for (int i = 0; i < 7; i++) {
        if (player.position == snakes[i]) {
            player.position = snakeEnds[i];
            return;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (player.position == ladders[i]) {
            player.position = ladderEnds[i];
            return;
        }
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Snakes and Ladders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 24);

    srand(time(0));
    Player player1 = {1};
    Player player2 = {1};

    boardTexture = IMG_LoadTexture(renderer, "image.png");
    backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    playerTextures[0] = IMG_LoadTexture(renderer, "player1.png");
    playerTextures[1] = IMG_LoadTexture(renderer, "player2.png");

    for (int i = 0; i < 6; i++) {
        std::string filePath = "images/dice_" + std::to_string(i + 1) + ".png";
        diceTextures[i] = IMG_LoadTexture(renderer, filePath.c_str());
    }

    bool quit = false;
    SDL_Event e;
    int turn = 1;
    int targetPos = 1;
    bool isMoving = false;
    int currentRoll = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (!gameWon && !isMoving && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                currentRoll = rollDice();
                Player& currentPlayer = (turn == 1 ? player1 : player2);
                targetPos = currentPlayer.position + currentRoll;
                if (targetPos > 64) targetPos = 64;
                isMoving = true;
            }
        }

        if (isMoving) {
            Player& currentPlayer = (turn == 1 ? player1 : player2);
            if (currentPlayer.position < targetPos) {
                currentPlayer.position++;
                SDL_Delay(150);
            } else {
                checkSnakesAndLadders(currentPlayer);
                if (currentPlayer.position == 64) {
                    gameWon = true;
                    winner = (turn == 1 ? "Player 1" : "Player 2") + std::string(" Wins!");
                }
                if (!gameWon && currentRoll != 6) {
                    turn = 3 - turn;
                }
                isMoving = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBackground(renderer);
        drawBoard(renderer);
        drawDice(renderer);
        drawPlayer(renderer, player1, 0);
        drawPlayer(renderer, player2, 1);

        if (gameWon) {
            renderText(renderer, winner, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50);
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(diceTextures[i]);
    }
    SDL_DestroyTexture(playerTextures[0]);
    SDL_DestroyTexture(playerTextures[1]);
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
