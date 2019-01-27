// Gameplay Screen

#include "game.h"
#include "map.h"

#include <math.h>
#include <stdio.h>

typedef struct Player {
	Rectangle bbox;
	Vector2 acc;
	Vector2 vel;
} Player;

static struct Player player;


// Module variables
#define MAX_BUILDINGS 100
static Camera2D camera;
static Rectangle buildings[MAX_BUILDINGS];
static Color buildColors[MAX_BUILDINGS];
static Map* map;
static int mapWidth;
static bool debugScreen = false;
const int TILE_SIZE = 21;

static Texture2D spritesheet;

static Texture2D characterSprites;

void InitGameplay(Game* game) {
	// Load tiled map and spritesheet
	map = LoadMap("res", "testmap.json");
	spritesheet = map->spritesheetTexture;
	mapWidth = map->tiledMap->width * TILE_SIZE;
	printf("width: %d\n", mapWidth);
	DrawMap(map);

	// Load Player spritesheet
	// TODO: Clean this mess up?
	{
		int i = 0;
		Image img = GenImageColor(21*10, 21, BLANK);
		Image tmp = GenImageColor(TILE_SIZE, TILE_SIZE, BLANK);
		const static int playerWalkAnim[] = {28, 29};
		const static int playerIdleAnim[] = {19, 20};
		Rectangle sourceRec, destRec;

		// Walk right
		sourceRec = MapTilesheetSourceRec(map, playerWalkAnim[0]);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, map->spritesheetImage, sourceRec, destRec);
		sourceRec = MapTilesheetSourceRec(map, playerWalkAnim[1]);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, map->spritesheetImage, sourceRec, destRec);

		// Walk left: flip them
		sourceRec = MapTilesheetSourceRec(map, playerWalkAnim[0]);
		destRec = (Rectangle) { 0, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&tmp, map->spritesheetImage, sourceRec, destRec);
		ImageFlipHorizontal(&tmp);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, tmp, (Rectangle) {0.0,0.0,TILE_SIZE,TILE_SIZE}, destRec);
		// TODO: Better image clearing?
		UnloadImage(tmp);
		tmp = GenImageColor(TILE_SIZE, TILE_SIZE, BLANK);
		// ImageDrawRectangle(&tmp, (Rectangle) {0.0,0.0,TILE_SIZE,TILE_SIZE}, BLANK);

		sourceRec = MapTilesheetSourceRec(map, playerWalkAnim[1]);
		destRec = (Rectangle) { 0, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&tmp, map->spritesheetImage, sourceRec, destRec);
		ImageFlipHorizontal(&tmp);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, tmp, (Rectangle) {0.0,0.0,TILE_SIZE,TILE_SIZE}, destRec);
		ImageDrawRectangle(&tmp, (Rectangle) {0.0,0.0,TILE_SIZE,TILE_SIZE}, BLANK);

		// IDLE
		sourceRec = MapTilesheetSourceRec(map, playerIdleAnim[0]);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, map->spritesheetImage, sourceRec, destRec);
		sourceRec = MapTilesheetSourceRec(map, playerIdleAnim[1]);
		destRec = (Rectangle) { i++ * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
		ImageDraw(&img, map->spritesheetImage, sourceRec, destRec);

		// TODO: POT?
		characterSprites = LoadTextureFromImage(img);

		UnloadImage(img);
	}

	player.bbox = (Rectangle) { 50, game->canvasHeight/2, 19, 19 };
	player.vel = (Vector2) { 0, 0 };

    camera.target = (Vector2){ player.bbox.x + 10, player.bbox.y + 10 };
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

	int spacing = 0;
    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].width = GetRandomValue(50, 200);
        buildings[i].height = GetRandomValue(100, 800);
        buildings[i].y = game->canvasHeight - 130 - buildings[i].height;
        buildings[i].x = -6000 + spacing;

        spacing += buildings[i].width;
        
        buildColors[i] = (Color){ GetRandomValue(200, 240), GetRandomValue(200, 240), GetRandomValue(200, 250), 255 };
    }
}
void DeinitGameplay(Game* game) {
	UnloadMap(map);
}

static bool leftWall;



void UpdateGameplay(Game* game) {

	// Camera target follows player
	// camera.target = (Vector2){ player.bbox.x + player.bbox.width , player.bbox.y + player.bbox.height };

	// Camera rotation controls

	if (IsKeyDown(KEY_W)) camera.offset.y+=2;
	if (IsKeyDown(KEY_A)) camera.offset.x+=2;
	if (IsKeyDown(KEY_S)) camera.offset.y-=2;
	if (IsKeyDown(KEY_D)) camera.offset.x-=2;


	// Player input
	bool wantJump = false;
	static bool hitGround = false;
	static bool hitCeiling = false;
	{

		if (IsKeyDown(KEY_RIGHT)) {
			player.vel.x = 7.0f;
		}
		if (IsKeyDown(KEY_LEFT)) {
			player.vel.x = -7.0f;
		}

		wantJump = IsKeyPressed(KEY_SPACE);

		if (IsKeyPressed(KEY_F1)) {
			debugScreen = !debugScreen;
		}
		/* if (onGround) { */
		/* 	player.bbox.y = playerBL.y*21-player.bbox.height; */
		/* 	player.vel.y = 0; */
		/* 	if (IsKeyPressed(KEY_SPACE)) { */
		/* 		player.vel.y = -10.0; */
		/* 	} */
		/* } else { */
		/* 	if (IsKeyReleased(KEY_SPACE) && player.vel.y < -5.0) { */
		/* 		player.vel.y = -5.0; */
		/* 	} */
		/* } */


	}

	const int NUM_STEPS = 10;
	// Player physics step
	{
		static Player oldPlayer = {};


		// Jump
		if (hitGround && wantJump) {
			player.vel.y = -5.0f;
		}

		// Handle X movement
		float movedX = 0.0f;
		for (int i = 0; i < NUM_STEPS; i++)
		{
			oldPlayer = player;
			// Friction
			
			if (player.vel.x > 0.1 || player.vel.x < -0.1) {
				player.vel.x *= 0.8;
			} else {
				player.vel.x = 0;
			}

			// Try to move player in X direction
			player.bbox.x += player.vel.x / (float)NUM_STEPS;
			movedX += player.vel.x / (float)NUM_STEPS;

			// Query map tiles after movement
			// TODO: Pack into function/Cleanup
			const Vector2 playerTL = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x, player.bbox.y});
			const Vector2 playerTR = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x+player.bbox.width, player.bbox.y});
			const Vector2 playerBL = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x, player.bbox.y+player.bbox.height});
			const Vector2 playerBR = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x+player.bbox.width, player.bbox.y+player.bbox.height});
			int tileTL = MapGetTileVec(map, playerTL);
			int tileTR = MapGetTileVec(map, playerTR);
			int tileBL = MapGetTileVec(map, playerBL);
			int tileBR = MapGetTileVec(map, playerBR);

			leftWall = tileTL > 0 || tileBL > 0;
			const bool rightWall = tileTR > 0 || tileBR > 0;

			if (leftWall || rightWall) {
				player.bbox.x = oldPlayer.bbox.x;
				player.vel.x = 0.0f;
			}	
		}

		for (int i = 0; i < NUM_STEPS; i++)
		// Handle Y movement
		{
			oldPlayer = player;
			// Apply gravity
			player.vel.y += 0.015f;

			const float maxYSpeed = 7.0f;
			if (player.vel.y > maxYSpeed)
				player.vel.y = maxYSpeed;
			if (player.vel.y < -maxYSpeed)
				player.vel.y = -maxYSpeed;

			// Try to move player in Y direction
			const float stepY = player.vel.y / (float)NUM_STEPS;
			/* printf("stepY: %f\n", stepY); */
			player.bbox.y += stepY;

			// Query map tiles after movement
			// TODO: Pack into function/Cleanup
			const Vector2 playerTL = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x, player.bbox.y});
			const Vector2 playerTR = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x+player.bbox.width, player.bbox.y});
			const Vector2 playerBL = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x, player.bbox.y+player.bbox.height});
			const Vector2 playerBR = MapWorldPosToMapPosVec(map, (Vector2) {player.bbox.x+player.bbox.width, player.bbox.y+player.bbox.height});
			int tileTL = MapGetTileVec(map, playerTL);
			int tileTR = MapGetTileVec(map, playerTR);
			int tileBL = MapGetTileVec(map, playerBL);
			int tileBR = MapGetTileVec(map, playerBR);

			// Y collision detection
			hitGround =  tileBL > 0 || tileBR > 0;
			hitCeiling = tileTL > 0 || tileTR > 0;

			// If we hit the ground, revert to old position
			if (hitGround || hitCeiling) {
				player.bbox.y = oldPlayer.bbox.y;
				player.vel.y = 0.0f;
			}
		}

		// Camera system
		{
			float cameraX = -camera.offset.x;
			if (movedX != 0.0f) {
				/* printf("movedX: %f\n", movedX); */
				float xDist = player.bbox.x - cameraX;
				/* printf("xDist: %f\n", xDist); */
				if (xDist > 450.0f && cameraX < mapWidth-800.0f) {
					camera.offset.x -= movedX;
				} else if(xDist < 350.0f && cameraX > 0.0f)  {
					camera.offset.x -= movedX;
				}
			}
		}

	}
}

/* void SawWavePoints(Vector2 *points, int len, int period) { */
/* 	(void)period; */
/* 	for (int x = 0; x <= len; x++) { */
/* 		points[x] = (Vector2) { x*10.0f, (x % 2 == 0 ? 1 : 0) * 10.0f }; */
/* 		printf("points[%d] = %f, %f\n", x, points[x].x, points[x].y); */
/* 	} */
/* } */

void RecWavePoints(Vector2 *points, int len, float period) {
	for (int x = 0; x <= len; x+=2) {
		if (x % 4 == 0) {
			points[x] =   (Vector2) { x*period, 0.0f };
			points[x+1] = (Vector2) { x*period, 10.0f };
		} else {
			points[x] =   (Vector2) { x*period, 10.0f };
			points[x+1] = (Vector2) { x*period, 0.0f };
		}

		/* printf("points[%d] = %f, %f\n", x, points[x].x, points[x].y); */
	}
}

void DrawGameplay(Game* game) {

	Vector2 mousePos = GetMousePosition();
	mousePos.x *= 0.4166666666666667;
	mousePos.y *= 0.4166666666666667;
	Vector2 mouseTilePos = MapWorldPosToMapPosVec(map, mousePos);

	ClearBackground(RAYWHITE);
	BeginMode2D(camera);
	{
		DrawMap(map);

		// TODO: Rectangle Gun
		/* { */
		/* 	static float period = 0.0f; */
		/* 	if (IsKeyDown(KEY_UP)) { */
		/* 		period+=0.1; */
		/* 	} else if (IsKeyDown(KEY_DOWN)) { */
		/* 		period-=0.1; */
		/* 	} */
		/* 	Vector2 recWave[10]; */
		/* 	RecWavePoints(recWave, 10, period); */
		/* 	DrawPolyExLines(recWave, 10, BLUE); */
		/* } */
		/* { */
		/* 	Vector2 recWave[4]; */
		/* 	recWave[0] = (Vector2) {10,1}; */
		/* 	recWave[1] = (Vector2) {20,1}; */
		/* 	recWave[2] = (Vector2) {30,1}; */
		/* 	recWave[3] = (Vector2) {40,1}; */
		/* 	DrawPolyExLines(recWave, 4, RED); */
		/* } */

		// Draw Player
		{
			static int tick = 0;
			static int frame = 0;
			static int freq = 15;

			if (++tick > freq) {
				tick = 0;
				if (++frame > 1) {
					frame = 0;
				}
			}

			int offset = 0;
			if (player.vel.x > 0.0f) {
				// TODO: Walk right
				freq = 15;
			} else if(player.vel.x < 0.0f) {
				// TODO: Walk left
				offset = 42;
				freq = 15;
			} else {
				// TODO: IDLE
				offset = 84;
				freq = 60;
			}

			const Rectangle sourceRec = (Rectangle) { offset+frame*TILE_SIZE, 0, 21, 21};
			const Vector2 pos = (Vector2) { player.bbox.x-2, player.bbox.y-2 };

			DrawTextureRec(characterSprites, sourceRec, pos, WHITE);
		}

		/* DrawLine(camera.target.x, -game->canvasHeight*10, camera.target.x, game->canvasHeight*10, GREEN); */
		/* DrawLine(-game->canvasWidth*10, camera.target.y, game->canvasWidth*10, camera.target.y, GREEN); */

		if (false) {
			// Draw Grid
			for (int x = 0; x < 64; x++) {
				for (int y = 0; y < 64; y++) {
					DrawRectangleLines(x*21, y*21, 21, 21, GRAY);
					if (mouseTilePos.x == x && mouseTilePos.y == y) {
						DrawRectangle(x*21, y*21, 21, 21, (Color) {0,255,0,128});
					}
				}
			}
		}

	}
	EndMode2D();

	// Debug printing
	if (debugScreen) {
		char buf[256];
		snprintf(buf, 256, "MousePos: (%f, %f)", mousePos.x, mousePos.y);
		DrawText(buf, 10, 10, 10, BLACK);

		snprintf(buf, 256, "tilePos: (%f, %f)", mouseTilePos.x, mouseTilePos.y);
		DrawText(buf, 10, 20, 10, BLACK);
		snprintf(buf, 256, "tileData: %d", MapGetTile(map, mouseTilePos.x, mouseTilePos.y));
		DrawText(buf, 10, 30, 10, BLACK);

		snprintf(buf, 256, "cameraOffset: (%f, %f)", camera.offset.x, camera.offset.y);
		DrawText(buf, 10, 40, 10, BLACK);

		if (leftWall) {
			DrawText("Left Wall!", 10, 50, 10, RED);
		}
	}

}
