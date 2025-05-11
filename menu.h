#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

int showMenu(SDL_Renderer* renderer);
int showGameModeSelection(SDL_Renderer* renderer);
int showCharacterSelection(SDL_Renderer* renderer, int playerNum, bool takenChars[4], SDL_Texture* bgTexture);

#endif // MENU_H
