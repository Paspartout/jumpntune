// All screens except gameplay

#include "game.h"
#include "screens.h"

#include <stdint.h>

static int counter = 0;

// Intro
// ==============================================
void InitIntro(Game* game) {
	counter = 0;
}
void DeinitIntro(Game* game) {
}
void UpdateIntro(Game* game) {
	if (counter++ == 1) {
		GameSwitchScreen(game, TITLE);
	}
}
void DrawIntro(Game* game) {
	ClearBackground(RAYWHITE);
	const char *msg = "Hello Jam!";
	const int fontSize = 32;
	static int msgWidth = 0;
	if (!msgWidth) { 
		msgWidth = MeasureText(msg, fontSize);
	}
	DrawText(msg, (game->canvasWidth/2)-msgWidth/2, game->canvasHeight/2, fontSize, BLACK);
}
// ==============================================


// Title
// ==============================================
enum TitleMenuItems { TITLE_START, TITLE_CREDITS, TITLE_EXIT, TITLE_NUM_ITEMS };
const char *MenuItems[] = { "Start", "Credits", "Exit" };
static unsigned titleSelection = 0;

void InitTitle(Game* game) {
}
void DeinitTitle(Game* game) {
}
void UpdateTitle(Game* game) {
	if (IsKeyPressed(KEY_DOWN) && titleSelection < TITLE_NUM_ITEMS) {
		titleSelection++;
	}
	if (IsKeyPressed(KEY_UP) && titleSelection > 0) {
		titleSelection--;
	}

	if (IsKeyPressed(KEY_ENTER)) {
		switch (titleSelection) {
			case TITLE_START:
				GameSwitchScreen(game, GAMEPLAY);
				break;
			case TITLE_CREDITS:
				GameSwitchScreen(game, CREDITS);
				break;
			case TITLE_EXIT:
				DeinitTitle(game);
				game->running = false;
				break;
		}
	}

}
void DrawTitle(Game* game) {
	ClearBackground(RAYWHITE);

	// Draw Title/Name
	{
		const char *msg = GAME_NAME;
		const int fontSize = 48;
		static int msgWidth = 0;
		if (!msgWidth) { 
			msgWidth = MeasureText(msg, fontSize);
		}
		DrawText(msg, (game->canvasWidth/2)-msgWidth/2, 32, fontSize, BLACK);
	}

	// Draw Menu
	{
		const int fontSize = 24;
		static int textWidth = 0;
		if (!textWidth) { 
			textWidth = MeasureText(MenuItems[1], fontSize); // Has to be longest word
		}

		const int startY = 128;
		for (int i = 0; i < TITLE_NUM_ITEMS; i++) {
			const Color textColor = titleSelection == i ? BLACK : GRAY;
			DrawText(MenuItems[i], (game->canvasWidth/2)-textWidth/2, startY+i*fontSize, fontSize, textColor);
		}
	}

}
// ==============================================


// Credits
// ==============================================

static AudioStream stream;
#define ADBUFSZ 4096
int16_t audiobuf[ADBUFSZ];
void InitCredits(Game* game) {
	stream = InitAudioStream(48000, 16, 1);
	PlayAudioStream(stream);
}
void DeinitCredits(Game* game) {
	CloseAudioStream(stream);
}
void UpdateCredits(Game* game) {

	static int period = 183;
	if (IsKeyPressed(KEY_UP)) {
		period--;
	} else if (IsKeyPressed(KEY_DOWN)) {
		period++;
	}

	int halfperiod = period/2;
	static float volume = 1.0f;

	if (IsAudioBufferProcessed(stream)) {
		if (volume > 0.0f)
			volume -= 0.1f;
		/* period+=10; */
		/* printf("period: %d\n", period); */
		static int x = 0;
		static bool state = 0;
		for (int i = 0; i < ADBUFSZ; i++) {
			if (++x == halfperiod) {
				state = !state;
				x = 0;
			}
			audiobuf[i] = state == 1 ? 4000 : -4000;
			audiobuf[i] = (int)((float)audiobuf[i] * volume);
			/* printf("%d\n", audiobuf[i]); */
		}
		UpdateAudioStream(stream, &audiobuf, ADBUFSZ);
	}

}
void DrawCredits(Game* game) {
	ClearBackground(RAYWHITE);
	// Draw Title/Name
	{
		const char *msg = "Credits";
		const int fontSize = 48;
		static int msgWidth = 0;
		if (!msgWidth) { 
			msgWidth = MeasureText(msg, fontSize);
		}
		DrawText(msg, (game->canvasWidth/2)-msgWidth/2, 32, fontSize, BLACK);
	}
}
// ==============================================

