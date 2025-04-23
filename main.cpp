#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;
const int GRID_SIZE = 8;
const int CELL_SIZE = (SCREEN_HEIGHT - 30) / GRID_SIZE;
const int BOARD_START_X = 170;
const int BOARD_START_Y = 15;


struct Player {
    int position;
};

SDL_Texture* diceTextures[6] = {nullptr};
SDL_Texture* backgroundTexture = nullptr;      // Background cho menu
SDL_Texture* gameBackgroundTexture = nullptr;  // Background trong game (trochoi)
SDL_Texture* playerTextures[2] = {nullptr};
TTF_Font* font = nullptr;
Mix_Music* bgMusic = nullptr;

int lastRoll = 1;
bool gameWon = false;
std::string winner = "";

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

void drawBackground(SDL_Renderer *renderer, SDL_Texture* bgTexture) {
    if (bgTexture) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
    }
}

void drawPlayer(SDL_Renderer *renderer, Player player, int index) {
    if (player.position > 0 && playerTextures[index]) {
        int row = getRow(player.position);
        int col = getCol(player.position);

        int size = CELL_SIZE ;
        SDL_Rect rect = {
            BOARD_START_X + col * CELL_SIZE + (CELL_SIZE - size) / 2,
            BOARD_START_Y + row * CELL_SIZE + (CELL_SIZE - size) / 2,
            size,
            size
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
            SDL_Delay(500);
            return;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (player.position == ladders[i]) {
            player.position = ladderEnds[i];
            SDL_Delay(500);
            return;
        }
    }
}

int showMenu(SDL_Renderer* renderer) {
    bool inMenu = true;
    SDL_Event e;

    SDL_Texture* menuBg = IMG_LoadTexture(renderer, "menu_bg.png");
    SDL_Texture* playBtn = IMG_LoadTexture(renderer, "play_button.png");
    SDL_Texture* howtoBtn = IMG_LoadTexture(renderer, "howto_button.png");

    SDL_Rect playRect = {SCREEN_WIDTH / 2 - (250 / 2) * 5, 0, 250 * 5, 140 * 5};
    SDL_Rect howtoRect = {SCREEN_WIDTH / 2 - 120 * 5, 360 - 130 * 5 / 2, 240 * 5, 130 * 5};
    SDL_Rect menuBgRect = {SCREEN_WIDTH / 2 - (260 / 2) * 5, 0, 260 * 5, 140 * 5};
    while (inMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (x >= playRect.x && x <= playRect.x + playRect.w &&
                    y >= playRect.y && y <= playRect.y + playRect.h) {
                    return 0;
                }
                if (x >= howtoRect.x && x <= howtoRect.x + howtoRect.w &&
                    y >= howtoRect.y && y <= howtoRect.y + howtoRect.h) {
                    std::cout << "Hiển thị hướng dẫn chơi game\n";
                }
            }
        }

        SDL_RenderClear(renderer);
        drawBackground(renderer, backgroundTexture);
        if (menuBg) SDL_RenderCopy(renderer, menuBg, NULL, &menuBgRect);
        if (playBtn) SDL_RenderCopy(renderer, playBtn, NULL, &playRect);
        if (howtoBtn) SDL_RenderCopy(renderer, howtoBtn, NULL, &howtoRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBg);
    SDL_DestroyTexture(playBtn);
    SDL_DestroyTexture(howtoBtn);
    return -1;
}

int main(int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *window = SDL_CreateWindow("Snakes and Ladders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 24);
    srand(time(0));

    Player player1 = {1};
    Player player2 = {1};

    backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    gameBackgroundTexture = IMG_LoadTexture(renderer, "trochoi.png");
    playerTextures[0] = IMG_LoadTexture(renderer, "player1.png");
    playerTextures[1] = IMG_LoadTexture(renderer, "player2.png");

    for (int i = 0; i < 6; i++) {
        std::string filePath = "images/dice_" + std::to_string(i + 1) + ".png";
        diceTextures[i] = IMG_LoadTexture(renderer, filePath.c_str());
    }

    bgMusic = Mix_LoadMUS("music.mp3");
    if (bgMusic) Mix_PlayMusic(bgMusic, -1);

    if (showMenu(renderer) != 0) {
        Mix_FreeMusic(bgMusic);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 0;
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
                if (targetPos > 64) targetPos = currentPlayer.position;
                isMoving = true;
            }
        }

        if (isMoving) {
            Player& currentPlayer = (turn == 1 ? player1 : player2);
            if (currentPlayer.position < targetPos) {
                currentPlayer.position++;
                SDL_Delay(500);
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
        drawBackground(renderer, gameBackgroundTexture); // Vẽ background toàn màn hình
        drawDice(renderer);
        drawPlayer(renderer, player1, 0);
        drawPlayer(renderer, player2, 1);

        if (gameWon) {
            renderText(renderer, winner, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50);
        } else {
            renderText(renderer, "Move:", 20, 150);
            SDL_Rect turnPlayerRect = {90, 135, 45, 45};
            SDL_RenderCopy(renderer, playerTextures[turn - 1], NULL, &turnPlayerRect);
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 6; i++) SDL_DestroyTexture(diceTextures[i]);
    SDL_DestroyTexture(playerTextures[0]);
    SDL_DestroyTexture(playerTextures[1]);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(gameBackgroundTexture);

    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
