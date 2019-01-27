// All screens except gameplay

#include "game.h"
#include "screens.h"

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
		const int fontSize = 64;
		static int msgWidth = 0;
		if (!msgWidth) { 
			msgWidth = MeasureText(msg, fontSize);
		}
		DrawText(msg, (game->canvasWidth/2)-msgWidth/2, 60, fontSize, BLACK);
	}

	// Draw Menu
	{
		const int fontSize = 32;
		static int textWidth = 0;
		if (!textWidth) { 
			textWidth = MeasureText(MenuItems[1], fontSize); // Has to be longest word
		}

		const int startY = 200;
		for (int i = 0; i < TITLE_NUM_ITEMS; i++) {
			const Color textColor = titleSelection == i ? BLACK : GRAY;
			DrawText(MenuItems[i], (game->canvasWidth/2)-textWidth/2, startY+i*fontSize, fontSize, textColor);
		}
	}

}
// ==============================================


// Credits
// ==============================================
void InitCredits(Game* game) {
}
void DeinitCredits(Game* game) {
}
void UpdateCredits(Game* game) {
}
void DrawCredits(Game* game) {
}
// ==============================================

