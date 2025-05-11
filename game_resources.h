#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>

struct Player;

extern SDL_Texture* diceTextures[6];
extern SDL_Texture* backgroundTexture;
extern SDL_Texture* gameBackgroundTexture;
extern SDL_Texture* onePlayerTexture;
extern SDL_Texture* twoPlayersTexture;
extern SDL_Texture* playerTextures[4];
extern SDL_Texture* winTextures[4];
extern SDL_Texture* howtoTexture;
extern SDL_Texture* player1Bg;
extern SDL_Texture* player2Bg;
extern TTF_Font* font;
extern Mix_Music* bgMusic;
extern SDL_Texture* soundOnTexture;
extern SDL_Texture* soundOffTexture;
extern SDL_Texture* endTexture;
extern SDL_Texture* homeButtonTexture;
extern SDL_Texture* replayButtonTexture;
extern Mix_Chunk* jumpSound;
extern Mix_Chunk* upSound;
extern Mix_Chunk* downSound;

extern int lastRoll;
extern bool gameWon;
extern bool isMuted;

void loadResources(SDL_Renderer* renderer);
void freeResources();
void drawBackground(SDL_Renderer* renderer, SDL_Texture* bgTexture);
void drawPlayer(SDL_Renderer* renderer, Player player, int index);
void drawDice(SDL_Renderer* renderer);
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y);

#endif // GAME_RESOURCES_H
