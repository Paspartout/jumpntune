#include <raylib.h>

#include "cute_tiled.h"

typedef struct Map {
	cute_tiled_map_t* tiledMap;
	cute_tiled_tileset_t* tiledTileset;
	cute_tiled_layer_t* collisionLayer;
	cute_tiled_layer_t* objectLayer;

	Texture2D spritesheetTexture;
	Image spritesheetImage;
	// TODO: Add textures etc
} Map;

typedef enum MapTileCollisionType {
	TileEmpty,
	TileFull,
	TileOneWay,
} MapTileCollisionType;

typedef enum MapTileObjectType {
	TileNone,
	TileCheckPoint,
} MapTileObjectType;

Rectangle MapTilesheetSourceRec(const Map* map, const int tid);
Map* LoadMap(const char *basePath, const char* mapPath);
void UnloadMap(Map* map);
void DrawMap(Map *map);
Vector2 MapWorldPosToMapPos(Map* map, float worldX, float worldY);
Vector2 MapWorldPosToMapPosVec(Map* map, Vector2 worldPos);

int MapGetTile(Map* map, int tileX, int tileY);
int MapGetTileVec(Map* map, Vector2 tilePos);

MapTileCollisionType MapGetTileType(Map* map, int tileX, int tileY);
MapTileCollisionType MapGetTileTypeVec(Map* map, Vector2 tilePos);


MapTileObjectType MapGetObjectType(Map* map, int tileX, int tileY);
MapTileObjectType MapGetObjectTypeVec(Map* map, Vector2 tilePos);
