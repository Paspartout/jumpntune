#include <raylib.h>
#include <stdio.h>

#include "game.h"

#include "screens.h"
const GameFunction initFunctions[NUM_SCREENS] = { 0, InitIntro, InitTitle, InitGameplay, InitCredits };
const GameFunction deinitFunctions[NUM_SCREENS] = { 0, DeinitIntro, DeinitTitle, DeinitGameplay, DeinitCredits };
const GameFunction updateFunctions[NUM_SCREENS] = { 0, UpdateIntro, UpdateTitle, UpdateGameplay, UpdateCredits };
const GameFunction drawFunctions[NUM_SCREENS] = { 0, DrawIntro, DrawTitle, DrawGameplay, DrawCredits };

void GameTick(Game *game) {
	// Update
	// ========================================================================
	{
		if (WindowShouldClose()) {
			game->running = false;
		}
		if (IsKeyPressed(KEY_F)) {
			ToggleFullscreen();
		}

		// TODO: UpdateGame State
		updateFunctions[game->currentScreen](game);
	}

	// Draw
	// ========================================================================
	{
		BeginTextureMode(game->screenTexture);

		// Draw current Game state
		drawFunctions[game->currentScreen](game);

		EndTextureMode();

		BeginDrawing();
		// Draw and scale screen to window
		DrawTexturePro(game->screenTexture.texture, 
				(Rectangle){ 0, 0, game->screenTexture.texture.width, -game->screenTexture.texture.height },
				(Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() },
				(Vector2) { 0, 0 }, 0.0, WHITE);
		EndDrawing();

	}
}


// TODO: Consider adding fading transition
void GameSwitchScreen(Game* game, GameScreen nextScreen) {
	// Deinit old screen
	if (game->currentScreen != NONE) {
		deinitFunctions[game->currentScreen](game);
	}

	// Init new
	// TODO: Consider adding return values for e.g. loading checks
	initFunctions[nextScreen](game);

	game->currentScreen = nextScreen;
}
