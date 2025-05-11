#ifndef GAME_IMPLEMENTATION_H
#define GAME_IMPLEMENTATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "player.h"

void toggleSound();
void resetGame(Player& player1, Player& player2, int& turn);
bool startNewGame(SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers);
void movePlayer(Player& currentPlayer, int targetPos, Mix_Chunk* jumpSound);
void renderGame(SDL_Renderer* renderer, Player& player1, Player& player2, int turn);
void renderWinScreen(SDL_Renderer* renderer, Player& player1, Player& player2, int turn);
void renderGameplayUI(SDL_Renderer* renderer, Player& player1, Player& player2, int turn);
bool handleGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers);
bool handleEndGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers);
bool handlePlayingGameClick(SDL_Event& e, SDL_Renderer* renderer, Player& player1, Player& player2, int& turn, int& numPlayers);
bool runGame(SDL_Renderer* renderer);
void loadGameResources(SDL_Renderer* renderer);
void freeGameResources();

#endif // GAME_IMPLEMENTATION_H
