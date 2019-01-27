#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#include "game.h"

int main(int argc, char const* argv[])
{
	Game game;
	game.canvasWidth = 800;
	game.canvasHeight = 450;
	game.running = true;
	game.currentScreen = NONE;

	// Initialization
	// ========================================================================
	{
		InitWindow(1920, 1080, GAME_NAME);
        SetTargetFPS(60);
		SetConfigFlags(FLAG_VSYNC_HINT);

		// Setup screenTexture for proper scaling
		game.screenTexture = LoadRenderTexture(game.canvasWidth, game.canvasHeight);
		SetTextureFilter(game.screenTexture.texture, 0);
		GameSwitchScreen(&game, GAMEPLAY);
	}

	while (game.running) {
		GameTick(&game);
	}

    // De-Initialization
	// ========================================================================
	{
		UnloadRenderTexture(game.screenTexture);
		CloseWindow();
	}

	return 0;
}



