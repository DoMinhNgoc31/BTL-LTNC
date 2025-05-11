#include "menu.h"
#include "constants.h"
#include "game_resources.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int showMenu(SDL_Renderer* renderer) {
    bool inMenu = true;
    SDL_Event e;

    SDL_Texture* menuBg = IMG_LoadTexture(renderer, "menu_bg.png");
    SDL_Texture* playBtn = IMG_LoadTexture(renderer, "play_button.png");
    SDL_Texture* howtoBtn = IMG_LoadTexture(renderer, "howto_button.png");
    SDL_Texture* backBtn = IMG_LoadTexture(renderer, "back_button.png");
    soundOnTexture = IMG_LoadTexture(renderer, "sound_on.png");
    soundOffTexture = IMG_LoadTexture(renderer, "sound_off.png");

    SDL_Rect playRect = {SCREEN_WIDTH / 2 - (150 / 2), SCREEN_HEIGHT / 2 - 50, 150, 150};
    SDL_Rect howtoRect = {SCREEN_WIDTH / 2 - (307 / 2), SCREEN_HEIGHT / 2 + 140, 307, 83};
    SDL_Rect menuBgRect = {SCREEN_WIDTH / 2 - (709 / 2), 75, 709, 143};
    SDL_Rect soundRect = {40, SCREEN_HEIGHT - 120, 80, 80};

    while (inMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (x >= soundRect.x && x <= soundRect.x + soundRect.w &&
                    y >= soundRect.y && y <= soundRect.y + soundRect.h) {
                    isMuted = !isMuted;
                    if (isMuted) Mix_PauseMusic();
                    else Mix_ResumeMusic();
                }

                if (x >= playRect.x && x <= playRect.x + playRect.w &&
                    y >= playRect.y && y <= playRect.y + playRect.h) {
                    SDL_DestroyTexture(menuBg);
                    SDL_DestroyTexture(playBtn);
                    SDL_DestroyTexture(howtoBtn);
                    SDL_DestroyTexture(backBtn);
                    return 0;
                }

                if (x >= howtoRect.x && x <= howtoRect.x + howtoRect.w &&
                    y >= howtoRect.y && y <= howtoRect.y + howtoRect.h) {

                    bool inHowto = true;
                    SDL_Rect howtoDestRect = {SCREEN_WIDTH / 2 - (633 / 2), 240, 633, 424};
                    SDL_Rect backRect = {120, 230, 84, 84};

                    while (inHowto) {
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_QUIT) return -1;

                            if (e.type == SDL_MOUSEBUTTONDOWN) {
                                int mx = e.button.x;
                                int my = e.button.y;

                                if (mx >= backRect.x && mx <= backRect.x + backRect.w &&
                                    my >= backRect.y && my <= backRect.y + backRect.h) {
                                    inHowto = false;
                                }

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

int showGameModeSelection(SDL_Renderer* renderer) {
    SDL_Texture* modeBg = IMG_LoadTexture(renderer, "mode_select_bg.png");
    onePlayerTexture = IMG_LoadTexture(renderer, "1player.png");
    twoPlayersTexture = IMG_LoadTexture(renderer, "2players.png");

    SDL_Rect bgRect = {0, 0, 900, 700};
    SDL_Rect onePlayerRect = {SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 15, 332, 88};
    SDL_Rect twoPlayersRect = {SCREEN_WIDTH / 2 - 215, SCREEN_HEIGHT / 2 + 108, 332, 88};

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (x >= onePlayerRect.x && x <= onePlayerRect.x + onePlayerRect.w &&
                    y >= onePlayerRect.y && y <= onePlayerRect.y + onePlayerRect.h) {
                    return 1;
                }
                if (x >= twoPlayersRect.x && x <= twoPlayersRect.x + twoPlayersRect.w &&
                    y >= twoPlayersRect.y && y <= twoPlayersRect.y + twoPlayersRect.h) {
                    return 2;
                }
            }
        }
        if (modeBg) SDL_RenderCopy(renderer, modeBg, NULL, &bgRect);
        if (onePlayerTexture) SDL_RenderCopy(renderer, onePlayerTexture, NULL, &onePlayerRect);
        if (twoPlayersTexture) SDL_RenderCopy(renderer, twoPlayersTexture, NULL, &twoPlayersRect);
        SDL_RenderPresent(renderer);
    }
    return -1;
}

int showCharacterSelection(SDL_Renderer* renderer, int playerNum, bool takenChars[4], SDL_Texture* bgTexture) {
    SDL_Texture* localTextures[3];
    localTextures[0] = IMG_LoadTexture(renderer, "char1.png");
    localTextures[1] = IMG_LoadTexture(renderer, "char2.png");
    localTextures[2] = IMG_LoadTexture(renderer, "char3.png");

    SDL_Rect charRects[3] = {
        {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2, 200, 200},
        {SCREEN_WIDTH / 2 - 100,  SCREEN_HEIGHT / 2 + 20, 200, 200},
        {SCREEN_WIDTH / 2 + 100,  SCREEN_HEIGHT / 2 + 5, 200, 200},
    };

    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                for (int i = 0; i < 3; ++i) {
                    if (takenChars[i]) continue;
                    if (x >= charRects[i].x && x <= charRects[i].x + charRects[i].w &&
                        y >= charRects[i].y && y <= charRects[i].y + charRects[i].h) {
                        for (int j = 0; j < 3; ++j) {
                            if (localTextures[j]) SDL_DestroyTexture(localTextures[j]);
                        }
                        return i;
                    }
                }
            }
        }
        SDL_Rect bgRect = {0, 0, 900, 700};
        if (bgTexture)
            SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);

        for (int i = 0; i < 3; ++i) {
            if (!takenChars[i] && localTextures[i])
                SDL_RenderCopy(renderer, localTextures[i], NULL, &charRects[i]);
        }

        SDL_RenderPresent(renderer);
    }

    return -1;
}
