#include "constants.h"
#include "game_logic.h"
#include "game_resources.h"
#include <cstdlib>
#include <SDL2/SDL.h>

int rollDice() {
    lastRoll = rand() % 6 + 1;
    return lastRoll;
}

int getRow(int pos) {
    return (GRID_SIZE - 1) - ((pos - 1) / GRID_SIZE);
}

int getCol(int pos) {
    int row = (pos - 1) / GRID_SIZE;
    return (row % 2 == 0) ? (pos - 1) % GRID_SIZE : GRID_SIZE - 1 - (pos - 1) % GRID_SIZE;
}

int rollDiceAnimation(SDL_Renderer* renderer, Player& player1, Player& player2) {
    int finalRoll = rand() % 6 + 1;

    SDL_Rect diceRect = {37, 50, 80, 80};

    for (int i = 0; i < 14; i++) {
        int tempRoll = rand() % 6 + 1;
        SDL_RenderCopy(renderer, diceTextures[tempRoll - 1], NULL, &diceRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(50);
    }

    SDL_RenderCopy(renderer, diceTextures[finalRoll - 1], NULL, &diceRect);
    SDL_RenderPresent(renderer);
    SDL_Delay(200);

    lastRoll = finalRoll;
    return finalRoll;
}

void checkSnakesAndLadders(Player& player, Mix_Chunk* downSound, Mix_Chunk* upSound) {
    int snakes[] = {35, 39, 44, 47, 57, 58, 60};
    int snakeEnds[] = {3, 4, 24, 34, 40, 45, 46};
    int ladders[] = {5, 6, 8, 15, 19, 23, 28, 42, 43, 50};
    int ladderEnds[] = {21, 11, 24, 31, 36, 40, 38, 59, 54, 63};

    for (int i = 0; i < 7; i++) {
        if (player.position == snakes[i]) {
            if (!isMuted) Mix_PlayChannel(-1, downSound, 0);
            player.position = snakeEnds[i];
            SDL_Delay(500);
            return;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (player.position == ladders[i]) {
            if (!isMuted) Mix_PlayChannel(-1, upSound, 0);
            player.position = ladderEnds[i];
            SDL_Delay(500);
            return;
        }
    }
}
