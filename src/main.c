#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#include "game.h"

int main(int argc, char const* argv[])
{
	Game game;
	game.canvasWidth = 480;
	game.canvasHeight = 270;
	game.screenWidth = 1920;
	game.screenHeight = 1080;
	game.running = true;
	game.currentScreen = NONE;

	// Initialization
	// ========================================================================
	{
		InitWindow(game.screenWidth, game.screenHeight, GAME_NAME);
        SetTargetFPS(60);
		SetConfigFlags(FLAG_VSYNC_HINT);
		InitAudioDevice();
		SetExitKey(KEY_F12);

		// Setup screenTexture for proper scaling
		game.screenTexture = LoadRenderTexture(game.canvasWidth, game.canvasHeight);
		SetTextureFilter(game.screenTexture.texture, 0);
		GameSwitchScreen(&game, TITLE);
	}

	while (game.running) {
		GameTick(&game);
	}

    // De-Initialization
	// ========================================================================
	{
		CloseAudioDevice();
		UnloadRenderTexture(game.screenTexture);
		CloseWindow();
	}

	return 0;
}



