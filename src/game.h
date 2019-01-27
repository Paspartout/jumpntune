#ifndef GAME_H
#define GAME_H

#include <raylib.h>

// Vector2i type
typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

#define GAME_NAME "Musicat"

enum GameScreen { NONE, INTRO, TITLE, GAMEPLAY, CREDITS, NUM_SCREENS };
typedef enum GameScreen GameScreen;

// Module variables
typedef struct Game {
	RenderTexture2D screenTexture;
	int canvasWidth;
	int canvasHeight;
	bool running;
	GameScreen currentScreen;
} Game;

// Main Loop Function
void GameTick(Game* game);

// Switch to given screen
void GameSwitchScreen(Game* game, GameScreen nextScreen);

// Function that works on the game, e.g. Update or Draw
typedef void (*GameFunction)(Game *game);

#endif
