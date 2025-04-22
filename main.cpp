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
const int CELL_SIZE = SCREEN_HEIGHT / GRID_SIZE;

struct Player {
    int position;
};

SDL_Texture* diceTextures[6] = {nullptr};
SDL_Texture* boardTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr; // Background cho menu
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

    // Xử lý rắn
    for (int i = 0; i < 7; i++) {
        if (player.position == snakes[i]) {
            // Di chuyển xuống dưới rắn trước
            player.position = snakeEnds[i];
            SDL_Delay(500); // Thêm delay để dừng lại ở dưới rắn trước khi di chuyển tiếp
            return;
        }
    }
    // Xử lý thang
    for (int i = 0; i < 10; i++) {
        if (player.position == ladders[i]) {
            // Di chuyển xuống dưới cầu thang trước
            player.position = ladderEnds[i];
            SDL_Delay(500); // Thêm delay để dừng lại ở dưới thang trước khi đi lên
            return;
        }
    }
}

int showMenu(SDL_Renderer* renderer) {
    bool inMenu = true;
    SDL_Event e;
    int selected = 0;

    while (inMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN:
                        selected = 1 - selected;
                        break;
                    case SDLK_RETURN:
                        return selected;
                }
            }
        }

        // Vẽ background cho menu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Set màu đen cho nền (bạn có thể thay đổi nếu cần)
        SDL_RenderClear(renderer);  // Xóa màn hình

        if (backgroundTexture) { // Kiểm tra nếu có background texture
            SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect); // Vẽ background
        }

        // Vẽ các lựa chọn menu
        renderText(renderer, "Snakes and Ladders", SCREEN_WIDTH / 2 - 150, 100);
        renderText(renderer, selected == 0 ? "> Start Game <" : "Start Game", SCREEN_WIDTH / 2 - 100, 250);
        renderText(renderer, selected == 1 ? "> Quit <" : "Quit", SCREEN_WIDTH / 2 - 100, 300);

        SDL_RenderPresent(renderer);  // Cập nhật màn hình
    }
    return -1;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    SDL_Window *window = SDL_CreateWindow("Snakes and Ladders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 24);
    srand(time(0));

    Player player1 = {1};
    Player player2 = {1};

    boardTexture = IMG_LoadTexture(renderer, "image.png");
    backgroundTexture = IMG_LoadTexture(renderer, "background.png"); // Tải background cho menu
    playerTextures[0] = IMG_LoadTexture(renderer, "player1.png");
    playerTextures[1] = IMG_LoadTexture(renderer, "player2.png");

    for (int i = 0; i < 6; i++) {
        std::string filePath = "images/dice_" + std::to_string(i + 1) + ".png";
        diceTextures[i] = IMG_LoadTexture(renderer, filePath.c_str());
    }

    bgMusic = Mix_LoadMUS("music.mp3");
    if (!bgMusic) {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
        Mix_PlayMusic(bgMusic, -1);
    }

    int menuChoice = showMenu(renderer);
    if (menuChoice == -1 || menuChoice == 1) {
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

                // Kiểm tra nếu vượt quá ô 64, giữ nguyên vị trí
                if (targetPos > 64) {
                    targetPos = currentPlayer.position; // Giữ nguyên vị trí nếu vượt quá ô 64
                }
                isMoving = true;
            }
        }

        if (isMoving) {
            Player& currentPlayer = (turn == 1 ? player1 : player2);

            // Di chuyển theo từng bước cho đến khi đến đúng ô đích
            if (currentPlayer.position < targetPos) {
                currentPlayer.position++;
                SDL_Delay(500); // Điều chỉnh độ trễ nếu muốn di chuyển chậm
            } else if (currentPlayer.position > targetPos) {
                currentPlayer.position--;
                SDL_Delay(500); // Điều chỉnh độ trễ nếu muốn di chuyển chậm
            } else {
                // Khi đến đúng ô đích
                checkSnakesAndLadders(currentPlayer);
                if (currentPlayer.position == 64) {
                    gameWon = true;
                    winner = (turn == 1 ? "Player 1" : "Player 2") + std::string(" Wins!");
                }
                if (!gameWon && currentRoll != 6) {
                    turn = 3 - turn;  // Chuyển lượt
                }
                isMoving = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBackground(renderer); // Vẽ background trong game
        drawBoard(renderer);
        drawDice(renderer);
        drawPlayer(renderer, player1, 0);
        drawPlayer(renderer, player2, 1);

        if (gameWon) {
    renderText(renderer, winner, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50);
} else {
    // Hiển thị chữ "Turn:"
    renderText(renderer, "Move:", 20, 150);

    // Vẽ hình ảnh người chơi hiện tại kế bên chữ "Turn:"
    SDL_Rect turnPlayerRect = {90, 135, 45, 45}; // vị trí và kích thước
    SDL_RenderCopy(renderer, playerTextures[turn - 1], NULL, &turnPlayerRect);
}
        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 6; i++) SDL_DestroyTexture(diceTextures[i]);
    SDL_DestroyTexture(playerTextures[0]);
    SDL_DestroyTexture(playerTextures[1]);
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(backgroundTexture); // Giải phóng texture của background

    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
