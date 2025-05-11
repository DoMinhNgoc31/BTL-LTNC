#include "game_implementation.h"
#include "constants.h"
#include "player.h"
#include "game_resources.h"
#include "game_logic.h"
#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>

void toggleSound() {
    isMuted = !isMuted;
    if (isMuted) Mix_PauseMusic();
    else Mix_ResumeMusic();
}

void resetGame(Player& player1, Player& player2, int& turn) {
    player1.position = 1;
    player2.position = 1;
    turn = 1;
    gameWon = false;
}

bool startNewGame(SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers) {
    if (showMenu(renderer) != 0) {
        return false;
    }

    numPlayers = showGameModeSelection(renderer);
    if (numPlayers == 0) {
        return false;
    }

    bool takenChars[4] = {false, false, false, false};

    int selectedChar1 = showCharacterSelection(renderer, 1, takenChars, player1Bg);
    if (selectedChar1 == -1) return false;
    player1.character = selectedChar1;
    takenChars[selectedChar1] = true;

    if (numPlayers == 2) {
        int selectedChar2 = showCharacterSelection(renderer, 2, takenChars, player2Bg);
        if (selectedChar2 == -1) return false;
        player2.character = selectedChar2;
        takenChars[selectedChar2] = true;
    } else {
        player2.character = 3;
    }

    resetGame(player1, player2, turn);
    return true;
}

void movePlayer(Player& currentPlayer, int targetPos, Mix_Chunk* jumpSound) {
    if (currentPlayer.position < targetPos) {
        currentPlayer.position++;
        if (!isMuted) Mix_PlayChannel(-1, jumpSound, 0);
        SDL_Delay(500);
    }
}

void renderGame(SDL_Renderer* renderer, Player& player1, Player& player2, int turn) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    drawBackground(renderer, gameBackgroundTexture);
    drawDice(renderer);
    drawPlayer(renderer, player1, 0);
    drawPlayer(renderer, player2, 1);

    if (gameWon) {
        renderWinScreen(renderer, player1, player2, turn);
    } else {
        renderGameplayUI(renderer, player1, player2, turn);
    }

    SDL_Rect soundRect = {40, SCREEN_HEIGHT - 120, 80, 80};
    SDL_RenderCopy(renderer, isMuted ? soundOffTexture : soundOnTexture, NULL, &soundRect);

    SDL_RenderPresent(renderer);
}

void renderWinScreen(SDL_Renderer* renderer, Player& player1, Player& player2, int turn) {
    SDL_Rect endRect = {0, 0, 900, 700};
    SDL_RenderCopy(renderer, endTexture, nullptr, &endRect);

    int winChar = (turn == 1 ? player1.character : player2.character);
    SDL_Rect winRect = {0, 0, 900, 700};
    SDL_RenderCopy(renderer, winTextures[winChar], nullptr, &winRect);

    SDL_Rect homeRect = {SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 100, 100, 100};
    SDL_Rect replayRect = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 + 100, 100, 100};
    SDL_RenderCopy(renderer, homeButtonTexture, NULL, &homeRect);
    SDL_RenderCopy(renderer, replayButtonTexture, NULL, &replayRect);
}

void renderGameplayUI(SDL_Renderer* renderer, Player& player1, Player& player2, int turn) {
    renderText(renderer, "Turn:", 25, 150);
    SDL_Rect turnPlayerRect = {90, 135, 45, 45};
    int currentCharacter = (turn == 1 ? player1.character : player2.character);
    SDL_RenderCopy(renderer, playerTextures[currentCharacter], NULL, &turnPlayerRect);

    SDL_Rect homeRect = {40, SCREEN_HEIGHT - 300, 80, 80};
    SDL_Rect replayRect = {40, SCREEN_HEIGHT - 210, 80, 80};
    SDL_RenderCopy(renderer, homeButtonTexture, NULL, &homeRect);
    SDL_RenderCopy(renderer, replayButtonTexture, NULL, &replayRect);
}

bool handleGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers) {
    int x = e.button.x;
    int y = e.button.y;

    SDL_Rect soundRect = {40, SCREEN_HEIGHT - 120, 80, 80};
    if (x >= soundRect.x && x <= soundRect.x + soundRect.w &&
        y >= soundRect.y && y <= soundRect.y + soundRect.h) {
        toggleSound();
        return true;
    }

    if (gameWon) {
        return handleEndGameClick(e, renderer, player1, player2, turn, numPlayers);
    } else {
        return handlePlayingGameClick(e, renderer, player1, player2, turn, numPlayers);
    }
}

bool handleEndGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers) {
    int x = e.button.x;
    int y = e.button.y;

    SDL_Rect homeRect = {SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 100, 100, 100};
    SDL_Rect replayRect = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 + 100, 100, 100};

    if (x >= homeRect.x && x <= homeRect.x + homeRect.w &&
        y >= homeRect.y && y <= homeRect.y + homeRect.h) {
        return startNewGame(renderer, player1, player2, turn, numPlayers);
    }

    if (x >= replayRect.x && x <= replayRect.x + replayRect.w &&
        y >= replayRect.y && y <= replayRect.y + replayRect.h) {
        resetGame(player1, player2, turn);
        return true;
    }

    return true;
}

bool handlePlayingGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers) {
    int x = e.button.x;
    int y = e.button.y;

    SDL_Rect homeRect = {40, SCREEN_HEIGHT - 300, 80, 80};
    SDL_Rect replayRect = {40, SCREEN_HEIGHT - 210, 80, 80};

    if (x >= homeRect.x && x <= homeRect.x + homeRect.w &&
        y >= homeRect.y && y <= homeRect.y + homeRect.h) {
        return startNewGame(renderer, player1, player2, turn, numPlayers);
    }

    if (x >= replayRect.x && x <= replayRect.x + replayRect.w &&
        y >= replayRect.y && y <= replayRect.y + replayRect.h) {
        resetGame(player1, player2, turn);
        return true;
    }

    return true;
}

bool runGame(SDL_Renderer* renderer) {
    Player player1 = {1, 0};
    Player player2 = {1, 0};
    int turn = 1;
    int numPlayers = 0;

    loadGameResources(renderer);
    if (bgMusic) Mix_PlayMusic(bgMusic, -1);

    if (!startNewGame(renderer, player1, player2, turn, numPlayers)) {
        freeGameResources();
        return false;
    }

    bool quit = false;
    SDL_Event e;
    int targetPos = 1;
    bool isMoving = false;
    int currentRoll = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (!handleGameClick(e, renderer, player1, player2, turn, numPlayers)) {
                    quit = true;
                }
            } else if (!gameWon && !isMoving) {
                if ((numPlayers == 2 && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) ||
                    (numPlayers == 1 && turn == 1 && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)) {
                    currentRoll = rollDiceAnimation(renderer, player1, player2);
                    Player& currentPlayer = (turn == 1 ? player1 : player2);
                    targetPos = currentPlayer.position + currentRoll;
                    if (targetPos > 64) targetPos = currentPlayer.position;
                    isMoving = true;
                }
            }
        }

        if (!gameWon && numPlayers == 1 && turn == 2 && !isMoving) {
            SDL_Delay(800); // delay cho máy cbi
            currentRoll = rollDiceAnimation(renderer, player1, player2);
            Player& currentPlayer = player2;
            targetPos = currentPlayer.position + currentRoll;
            if (targetPos > 64) targetPos = currentPlayer.position;
            isMoving = true;
        }

        if (isMoving) {
            Player& currentPlayer = (turn == 1 ? player1 : player2);
            if (currentPlayer.position < targetPos) {
                movePlayer(currentPlayer, targetPos, jumpSound);
            } else {
                checkSnakesAndLadders(currentPlayer, downSound, upSound);
                if (currentPlayer.position == 64) {
                    gameWon = true;
                }
                if (!gameWon && currentRoll != 6) {
                    turn = 3 - turn;
                }
                isMoving = false;
            }
        }

        renderGame(renderer, player1, player2, turn);
    }

    freeGameResources();
    return true;
}

void loadGameResources(SDL_Renderer* renderer) {
    backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    gameBackgroundTexture = IMG_LoadTexture(renderer, "trochoi.png");
    playerTextures[0] = IMG_LoadTexture(renderer, "char1.png");
    playerTextures[1] = IMG_LoadTexture(renderer, "char2.png");
    playerTextures[2] = IMG_LoadTexture(renderer, "char3.png");
    playerTextures[3] = IMG_LoadTexture(renderer, "char4.png");
    winTextures[0] = IMG_LoadTexture(renderer, "win1.png");
    winTextures[1] = IMG_LoadTexture(renderer, "win2.png");
    winTextures[2] = IMG_LoadTexture(renderer, "win3.png");
    winTextures[3] = IMG_LoadTexture(renderer, "win4.png");
    howtoTexture = IMG_LoadTexture(renderer, "howto.png");
    soundOnTexture = IMG_LoadTexture(renderer, "sound_on.png");
    soundOffTexture = IMG_LoadTexture(renderer, "sound_off.png");
    endTexture = IMG_LoadTexture(renderer, "end.png");
    homeButtonTexture = IMG_LoadTexture(renderer, "home_button.png");
    replayButtonTexture = IMG_LoadTexture(renderer, "replay_button.png");
    player1Bg = IMG_LoadTexture(renderer, "player1_bg.png");
    player2Bg = IMG_LoadTexture(renderer, "player2_bg.png");
    jumpSound = Mix_LoadWAV("jump.mp3");
    upSound = Mix_LoadWAV("up.mp3");
    downSound = Mix_LoadWAV("down.mp3");
    bgMusic = Mix_LoadMUS("music.mp3");
    for (int i = 0; i < 6; i++) {
        std::string filePath = "images/dice_" + std::to_string(i + 1) + ".png";
        diceTextures[i] = IMG_LoadTexture(renderer, filePath.c_str());
    }
}

void freeGameResources() {
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
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (jumpSound) Mix_FreeChunk(jumpSound);
    if (upSound) Mix_FreeChunk(upSound);
    if (downSound) Mix_FreeChunk(downSound);
}
