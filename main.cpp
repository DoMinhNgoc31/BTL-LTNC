#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

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
SDL_Texture* gameBackgroundTexture = nullptr;  // Background trong game
SDL_Texture* playerTextures[2] = {nullptr};
SDL_Texture* winTextures[2] = {nullptr};
SDL_Texture* howtoTexture = nullptr;            // Hình hướng dẫn
TTF_Font* font = nullptr;
Mix_Music* bgMusic = nullptr;
SDL_Texture* soundOnTexture = nullptr;
SDL_Texture* soundOffTexture = nullptr;
bool isMuted = false;

int lastRoll = 1;
bool gameWon = false;
string winner = "";

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

        int size = CELL_SIZE;
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
    SDL_Rect diceRect = {37, 50, 80, 80};
    if (diceTextures[lastRoll - 1]) {
        SDL_RenderCopy(renderer, diceTextures[lastRoll - 1], NULL, &diceRect);
    }
}

void renderText(SDL_Renderer* renderer, const string& text, int x, int y) {
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
    SDL_Texture* backBtn = IMG_LoadTexture(renderer, "back_button.png");
    soundOnTexture = IMG_LoadTexture(renderer, "sound_on.png");
    soundOffTexture = IMG_LoadTexture(renderer, "sound_off.png");

    SDL_Rect playRect = {SCREEN_WIDTH / 2 - (130 / 2), SCREEN_HEIGHT / 2 - 40, 130, 130};
    SDL_Rect howtoRect = {SCREEN_WIDTH / 2 - (434 / 2), SCREEN_HEIGHT / 2 + 130, 434, 73};
    SDL_Rect menuBgRect = {SCREEN_WIDTH / 2 - (709 / 2), 75, 709, 132};
    SDL_Rect soundRect = {40,SCREEN_HEIGHT- 120, 80, 80};  // Dùng chung

    while (inMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                // Xử lý nút âm thanh
                if (x >= soundRect.x && x <= soundRect.x + soundRect.w &&
                    y >= soundRect.y && y <= soundRect.y + soundRect.h) {
                    isMuted = !isMuted;
                    if (isMuted) Mix_PauseMusic();
                    else Mix_ResumeMusic();
                }

                // Xử lý nút Play
                if (x >= playRect.x && x <= playRect.x + playRect.w &&
                    y >= playRect.y && y <= playRect.y + playRect.h) {
                    SDL_DestroyTexture(menuBg);
                    SDL_DestroyTexture(playBtn);
                    SDL_DestroyTexture(howtoBtn);
                    SDL_DestroyTexture(backBtn);
                    return 0;
                }

                // Xử lý nút Howto
                if (x >= howtoRect.x && x <= howtoRect.x + howtoRect.w &&
                    y >= howtoRect.y && y <= howtoRect.y + howtoRect.h) {

                    // Hiển thị màn hình hướng dẫn
                    bool inHowto = true;
                    SDL_Rect howtoDestRect = {SCREEN_WIDTH / 2 - (633 / 2), 240, 633, 424};
                    SDL_Rect backRect = {120, 230, 84, 84};

                    while (inHowto) {
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_QUIT) return -1;

                            if (e.type == SDL_MOUSEBUTTONDOWN) {
                                int mx = e.button.x;
                                int my = e.button.y;

                                // Nút back
                                if (mx >= backRect.x && mx <= backRect.x + backRect.w &&
                                    my >= backRect.y && my <= backRect.y + backRect.h) {
                                    inHowto = false;
                                }

                                // Nút âm thanh (trong màn hình hướng dẫn)
                                if (mx >= soundRect.x && mx <= soundRect.x + soundRect.w &&
                                    my >= soundRect.y && my <= soundRect.y + soundRect.h) {
                                    isMuted = !isMuted;
                                    if (isMuted) Mix_PauseMusic();
                                    else Mix_ResumeMusic();
                                }
                            }

                            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                                inHowto = false;
                            }
                        }

                        SDL_RenderClear(renderer);
                        drawBackground(renderer, backgroundTexture);
                        if (menuBg) SDL_RenderCopy(renderer, menuBg, NULL, &menuBgRect);
                        if (howtoTexture) SDL_RenderCopy(renderer, howtoTexture, NULL, &howtoDestRect);
                        if (backBtn) SDL_RenderCopy(renderer, backBtn, NULL, &backRect);

                        // Vẽ nút âm thanh
                        SDL_RenderCopy(renderer, isMuted ? soundOffTexture : soundOnTexture, NULL, &soundRect);

                        SDL_RenderPresent(renderer);
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        drawBackground(renderer, backgroundTexture);
        if (menuBg) SDL_RenderCopy(renderer, menuBg, NULL, &menuBgRect);
        if (playBtn) SDL_RenderCopy(renderer, playBtn, NULL, &playRect);
        if (howtoBtn) SDL_RenderCopy(renderer, howtoBtn, NULL, &howtoRect);
        SDL_RenderCopy(renderer, isMuted ? soundOffTexture : soundOnTexture, NULL, &soundRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBg);
    SDL_DestroyTexture(playBtn);
    SDL_DestroyTexture(howtoBtn);
    SDL_DestroyTexture(backBtn);
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
    winTextures[0] = IMG_LoadTexture(renderer, "win1.png");
    winTextures[1] = IMG_LoadTexture(renderer, "win2.png");
    howtoTexture = IMG_LoadTexture(renderer, "howto.png");
    soundOnTexture = IMG_LoadTexture(renderer, "sound_on.png");
    soundOffTexture = IMG_LoadTexture(renderer, "sound_off.png");
     // Hiển thị nút âm thanh góc trên bên phải
    SDL_Rect soundRect = {SCREEN_WIDTH - 80, 20, 50, 50};
    SDL_RenderCopy(renderer, isMuted ? soundOffTexture : soundOnTexture, NULL, &soundRect);

    for (int i = 0; i < 6; i++) {
        string filePath = "images/dice_" + to_string(i + 1) + ".png";
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
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                SDL_Rect soundRect = {40,SCREEN_HEIGHT- 120, 80, 80};  // Cùng vị trí với nút vẽ

                if (x >= soundRect.x && x <= soundRect.x + soundRect.w &&
                    y >= soundRect.y && y <= soundRect.y + soundRect.h) {
                    isMuted = !isMuted;
                    if (isMuted) {
                        Mix_PauseMusic();
                    } else {
                        Mix_ResumeMusic();
                    }
                }
            }

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
                    winner = (turn == 1 ? "Player 1" : "Player 2") + string(" Wins!");
                }
                if (!gameWon && currentRoll != 6) {
                    turn = 3 - turn;
                }
                isMoving = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBackground(renderer, gameBackgroundTexture);
        drawDice(renderer);
        drawPlayer(renderer, player1, 0);
        drawPlayer(renderer, player2, 1);

        if (gameWon) {
            int winnerIndex = (turn == 1 ? 0 : 1);
            if (winTextures[winnerIndex]) {
                SDL_Rect winRect = {SCREEN_WIDTH / 2 - 825/2, SCREEN_HEIGHT / 2 - 400/2, 825, 465};
                SDL_RenderCopy(renderer, winTextures[winnerIndex], nullptr, &winRect);
            }
        } else {
            renderText(renderer, "Move:", 20, 150);
            SDL_Rect turnPlayerRect = {90, 135, 45, 45};
            SDL_RenderCopy(renderer, playerTextures[turn - 1], NULL, &turnPlayerRect);
        }
        // Hiển thị nút âm thanh góc trên bên phải
        SDL_Rect soundRect = {40,SCREEN_HEIGHT- 120, 80, 80};
        SDL_RenderCopy(renderer, isMuted ? soundOffTexture : soundOnTexture, NULL, &soundRect);

        SDL_RenderPresent(renderer);

    }

    for (int i = 0; i < 6; i++) SDL_DestroyTexture(diceTextures[i]);
    SDL_DestroyTexture(playerTextures[0]);
    SDL_DestroyTexture(playerTextures[1]);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(gameBackgroundTexture);
    SDL_DestroyTexture(howtoTexture);
    SDL_DestroyTexture(soundOnTexture);
    SDL_DestroyTexture(soundOffTexture);


    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
