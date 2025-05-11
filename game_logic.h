#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "player.h"

int rollDice();
int getRow(int pos);
int getCol(int pos);
int rollDiceAnimation(SDL_Renderer* renderer, Player& player1, Player& player2);
void checkSnakesAndLadders(Player& player, Mix_Chunk* downSound, Mix_Chunk* upSound);

#endif // GAME_LOGIC_H
