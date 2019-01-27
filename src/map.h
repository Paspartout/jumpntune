#include <raylib.h>

#include "cute_tiled.h"

typedef struct Map {
	cute_tiled_map_t* tiledMap;
	cute_tiled_tileset_t* tiledTileset;
	cute_tiled_layer_t* collisionLayer;

	Texture2D spritesheetTexture;
	Image spritesheetImage;
	// TODO: Add textures etc
} Map;

typedef enum MapTileType {
	TileEmpty,
	TileFull,
	TileOneWay,
} MapTileType;

Rectangle MapTilesheetSourceRec(const Map* map, const int tid);
Map* LoadMap(const char *basePath, const char* mapPath);
void UnloadMap(Map* map);
void DrawMap(Map *map);
Vector2 MapWorldPosToMapPos(Map* map, float worldX, float worldY);
Vector2 MapWorldPosToMapPosVec(Map* map, Vector2 worldPos);

int MapGetTile(Map* map, int tileX, int tileY);
int MapGetTileVec(Map* map, Vector2 tilePos);

MapTileType MapGetTileType(Map* map, int tileX, int tileY);
MapTileType MapGetTileTypeVec(Map* map, Vector2 tilePos);
