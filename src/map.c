#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#include "map.h"
#include <stdio.h>
#include <string.h>

Map* LoadMap(const char *basePath, const char* mapPath) {
	char buf[256];
	snprintf(buf, 256, "%s/%s", basePath, mapPath);

	Map* map = malloc(sizeof(Map));
	cute_tiled_map_t *tm = cute_tiled_load_map_from_file(buf, NULL);
	map->tiledMap = tm;

	// Load spritesheet as texture
	// TODO/NOTE: This uses only first spritesheet for now
	cute_tiled_tileset_t* tileset = tm->tilesets;


	map->tiledTileset = tileset;
	snprintf(buf, 256, "%s/%s", basePath, tileset->image.ptr);

	// Load spritesheet to image and texture
	map->spritesheetImage = LoadImage(buf);

	// TODO: Figure out why this doesn't work
	// NOTE: Probably because of image format not suport alpha!
	/* if (tileset->transparentcolor) { */
	/* 	Color colorKey; */
	/* 	colorKey.r = 0x5e; */
	/* 	colorKey.g = 0x81; */
	/* 	colorKey.b = 0xa2; */
	/* 	colorKey.a = 255; */
	/* 	ImageColorReplace(&img, colorKey, BLANK); */
	/* } */
	
	map->spritesheetTexture = LoadTextureFromImage(map->spritesheetImage);
	SetTextureFilter(map->spritesheetTexture, FILTER_POINT);

	// Find collision and object layer and assign them
	cute_tiled_layer_t* layer = tm->layers;
	map->collisionLayer = NULL;
	while (layer && map->collisionLayer == NULL) {
		for (int i = 0; i < layer->property_count; i++) {
			cute_tiled_property_t prop = layer->properties[i];
			if(prop.type == CUTE_TILED_PROPERTY_BOOL &&
			   strcmp(prop.name.ptr, "collision") == 0 &&
			   prop.data.boolean) {
			   map->collisionLayer = layer;
				break;
			}
		}
		layer = layer->next;
	}
	layer = tm->layers;
	map->objectLayer = NULL;
	while (layer && map->objectLayer == NULL) {
		for (int i = 0; i < layer->property_count; i++) {
			cute_tiled_property_t prop = layer->properties[i];
			if(prop.type == CUTE_TILED_PROPERTY_BOOL &&
			   strcmp(prop.name.ptr, "object") == 0 &&
			   prop.data.boolean) {
			   map->objectLayer = layer;
				break;
			}
		}
		layer = layer->next;
	}



	if (map->objectLayer == NULL) {
		TraceLog(LOG_ERROR, "No object layer found in map!\n");
		exit(-1);
	}

	if (map->collisionLayer == NULL) {
		TraceLog(LOG_ERROR, "No collision layer found in map!\n");
		exit(-1);
	}

	return map;
}

void UnloadMap(Map* map) {
	UnloadImage(map->spritesheetImage);
	UnloadTexture(map->spritesheetTexture);
	cute_tiled_free_map(map->tiledMap);
	free(map);
}

Rectangle MapTilesheetSourceRec(const Map* map, const int tid) {
	const cute_tiled_tileset_t *ts = map->tiledTileset;
	const int ts_columns = ts->columns;
	const int ts_margin = ts->margin;
	const int ts_spacing = ts->spacing;
	const int tw = ts->tilewidth;
	const int th = ts->tileheight;

	const int sx = ts_margin + ((tid % ts_columns) * tw) + ((tid % ts_columns) * ts_spacing);
	const int sy = ts_margin + ((tid / ts_columns) * tw) + ((tid / ts_columns) * ts_spacing);
	return (Rectangle) { .x = sx, .y = sy, .width = tw, .height = th};
}

void DrawMap(Map *map) {
	const cute_tiled_map_t *tm = map->tiledMap;
	const cute_tiled_tileset_t *ts = map->tiledTileset;
	const int w = tm->width;
	const int h = tm->height;
	const int ts_columns = ts->columns;
	const int ts_margin = ts->margin;
	const int ts_spacing = ts->spacing;
	const int tw = ts->tilewidth;
	const int th = ts->tileheight;

	// loop over the map's layers
	cute_tiled_layer_t* layer = tm->layers;

	while (layer)
	{
		int* data = layer->data;

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int tid = *data; // tile id
				if (tid > 0) { // 0 in tiled.json means no tile
					tid--;
					const int sx = ts_margin + ((tid % ts_columns) * tw) + ((tid % ts_columns) * ts_spacing);
					const int sy = ts_margin + ((tid / ts_columns) * tw) + ((tid / ts_columns) * ts_spacing);
					Rectangle sourceRec = { .x = sx, .y = sy, .width = tw, .height = th};
					// NOTE: +0.1f to fix rounding erros
					Rectangle destRec = { .x = x*tw, .y = y *th, .width = tw+0.1f, .height = th+0.1f};
					DrawTexturePro(map->spritesheetTexture, sourceRec, destRec, (Vector2) {0.0f, 0.0f}, 0.0f, WHITE);
				}
				data++;
			}
		}
		// TODO: Do something with the tile data

		layer = layer->next;
	}
}

Vector2 MapWorldPosToMapPos(Map* map, float worldX, float worldY) {
	const int tileSize = map->tiledTileset->tilewidth;
	// TODO:
	Vector2 mapPosition = {-1,0};
	const float mx = (worldX - mapPosition.x) / (float)tileSize;
	const float my = (worldY - mapPosition.y) / (float)tileSize;
	return (Vector2) {(int)mx, (int)my};
}


Vector2 MapWorldPosToMapPosVec(Map* map, Vector2 worldPos) {
	return MapWorldPosToMapPos(map, worldPos.x, worldPos.y);
}


// TODO: Specify layer?
int MapGetTile(Map* map, int tileX, int tileY) {
	const int w = map->tiledMap->width;
	const int h = map->tiledMap->height;
	if (tileX > w || tileY > h) {
		return -1;
	}
	int* data = map->collisionLayer->data;
	return data[tileX + tileY * w];
}

int MapGetTileVec(Map* map, Vector2 tilePos) {
	return MapGetTile(map, tilePos.x, tilePos.y);
}


const static int ONEWAY_TILES[] = {41, 158, 308, 339};

// TODO: Specify layer?, cleanup, code duplication ^^^^
MapTileCollisionType MapGetTileType(Map* map, int tileX, int tileY) {
	const int w = map->tiledMap->width;
	const int h = map->tiledMap->height;
	if (tileX > w || tileY > h) {
		return -1;
	}
	const int tid = map->collisionLayer->data[tileX + tileY * w]-1;
	if (tid <= 0) {
		return TileEmpty;
	}
	if (tid == 344) {
		return TileCheckPoint;
	}

	for (int i = 0; i < 4; i++) {
		if (tid == ONEWAY_TILES[i]) {
			return TileOneWay;
		}
	}

	return TileFull;
}

MapTileCollisionType MapGetTileTypeVec(Map* map, Vector2 tilePos) {
	return MapGetTileType(map, tilePos.x, tilePos.y);
}


MapTileObjectType MapGetObjectType(Map* map, int tileX, int tileY) {
	const int w = map->tiledMap->width;
	const int h = map->tiledMap->height;
	if (tileX > w || tileY > h) {
		return TileNone;
	}

	const int tid = map->objectLayer->data[tileX + tileY * w]-1;
	if (tid <= 0) {
		return TileNone;
	}
	if (tid == 344) {
		return TileCheckPoint;
	}

	return TileNone;
}

MapTileObjectType MapGetObjectTypeVec(Map* map, Vector2 tilePos) {
	return MapGetObjectType(map, tilePos.x, tilePos.y);
}



