#include "game_resources.h"
#include "player.h"
#include "constants.h"
#include <SDL2/SDL_image.h>
#include <iostream>

SDL_Texture* diceTextures[6] = {nullptr};
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* gameBackgroundTexture = nullptr;
SDL_Texture* onePlayerTexture = nullptr;
SDL_Texture* twoPlayersTexture = nullptr;
SDL_Texture* playerTextures[4] = {nullptr};
SDL_Texture* winTextures[4] = {nullptr};
SDL_Texture* howtoTexture = nullptr;
SDL_Texture* player1Bg = nullptr;
SDL_Texture* player2Bg = nullptr;
TTF_Font* font = nullptr;
Mix_Music* bgMusic = nullptr;
SDL_Texture* soundOnTexture = nullptr;
SDL_Texture* soundOffTexture = nullptr;
SDL_Texture* endTexture = nullptr;
SDL_Texture* homeButtonTexture = nullptr;
SDL_Texture* replayButtonTexture = nullptr;
Mix_Chunk* jumpSound = nullptr;
Mix_Chunk* upSound = nullptr;
Mix_Chunk* downSound = nullptr;

int lastRoll = 1;
bool gameWon = false;
bool isMuted = false;

void loadResources(SDL_Renderer* renderer) {
    backgroundTexture = IMG_LoadTexture(renderer, "menu_background.png");
    gameBackgroundTexture = IMG_LoadTexture(renderer, "game_background.png");
    howtoTexture = IMG_LoadTexture(renderer, "howto.png");
    player1Bg = IMG_LoadTexture(renderer, "player1_bg.png");
    player2Bg = IMG_LoadTexture(renderer, "player2_bg.png");
    soundOnTexture = IMG_LoadTexture(renderer, "sound_on.png");
    soundOffTexture = IMG_LoadTexture(renderer, "sound_off.png");
    endTexture = IMG_LoadTexture(renderer, "end_screen.png");
    homeButtonTexture = IMG_LoadTexture(renderer, "home_button.png");
    replayButtonTexture = IMG_LoadTexture(renderer, "replay_button.png");
}

void freeResources() {
    for (int i = 0; i < 6; i++) {
        if (diceTextures[i]) SDL_DestroyTexture(diceTextures[i]);
    }
    for (int i = 0; i < 4; i++) {
        if (playerTextures[i]) SDL_DestroyTexture(playerTextures[i]);
        if (winTextures[i]) SDL_DestroyTexture(winTextures[i]);
    }
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (gameBackgroundTexture) SDL_DestroyTexture(gameBackgroundTexture);
    if (onePlayerTexture) SDL_DestroyTexture(onePlayerTexture);
    if (twoPlayersTexture) SDL_DestroyTexture(twoPlayersTexture);
    if (howtoTexture) SDL_DestroyTexture(howtoTexture);
    if (player1Bg) SDL_DestroyTexture(player1Bg);
    if (player2Bg) SDL_DestroyTexture(player2Bg);
    if (soundOnTexture) SDL_DestroyTexture(soundOnTexture);
    if (soundOffTexture) SDL_DestroyTexture(soundOffTexture);
    if (endTexture) SDL_DestroyTexture(endTexture);
    if (homeButtonTexture) SDL_DestroyTexture(homeButtonTexture);
    if (replayButtonTexture) SDL_DestroyTexture(replayButtonTexture);
    if (font) TTF_CloseFont(font);
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (jumpSound) Mix_FreeChunk(jumpSound);
    if (upSound) Mix_FreeChunk(upSound);
    if (downSound) Mix_FreeChunk(downSound);
}

void drawBackground(SDL_Renderer *renderer, SDL_Texture* bgTexture) {
    if (bgTexture) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
    }
}

void drawPlayer(SDL_Renderer *renderer, Player player, int index) {
    if (player.position > 0 && playerTextures[index]) {
        int row = (GRID_SIZE - 1) - ((player.position - 1) / GRID_SIZE);
        int col;
        int rowNum = (player.position - 1) / GRID_SIZE;
        if (rowNum % 2 == 0) {
            col = (player.position - 1) % GRID_SIZE;
        } else {
            col = GRID_SIZE - 1 - (player.position - 1) % GRID_SIZE;
        }

        int size = CELL_SIZE;
        SDL_Rect rect = {
            BOARD_START_X + col * CELL_SIZE + (CELL_SIZE - size) / 2,
            BOARD_START_Y + row * CELL_SIZE + (CELL_SIZE - size) / 2,
            size, size
        };
        SDL_RenderCopy(renderer, playerTextures[player.character], NULL, &rect);
    }
}

void drawDice(SDL_Renderer *renderer) {
    SDL_Rect diceRect = {37, 50, 80, 80};
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
