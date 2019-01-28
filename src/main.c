#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#include "game.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif 

static Game game;

static void GGameTick() {
	GameTick(&game);
}

int main(int argc, char const* argv[])
{
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
		InitAudioDevice();
		SetExitKey(KEY_F12);

		// Setup screenTexture for proper scaling
		game.screenTexture = LoadRenderTexture(game.canvasWidth, game.canvasHeight);
		SetTextureFilter(game.screenTexture.texture, 0);
		GameSwitchScreen(&game, INTRO);
	}

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(GGameTick, 0, 1);
#else 
	SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
	while (game.running) {
		GGameTick();
	}
#endif
    // De-Initialization
	// ========================================================================
	{
		CloseAudioDevice();
		UnloadRenderTexture(game.screenTexture);
		CloseWindow();
	}

	return 0;
}



