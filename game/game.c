#include "ui.h"
#include "player.h"
#include "raylib.h"
#include "game.h"

void setPropRect(int propId, Rectangle* propsRect) {
	switch (propId) {
	case GRAVE_CROSS:
		//224 288
		propsRect->x = 224.0f;
		propsRect->y = 288.0f;
		propsRect->width = 32.0f;
		propsRect->height = 64.0f;
		break;
	case TREE1:
		propsRect->x = 0.0f;
		propsRect->y = 0.0f;
		propsRect->width = 32.0f * 5;
		propsRect->height = 32.0f * 5;
		break;
	case GRASS1:
		propsRect->x = 0.0f;
		propsRect->y = 384.0f;
		propsRect->width = 32.0f;
		propsRect->height = 32.0f;
		break;
	default:
		propsRect->x = 0.0f;
		propsRect->y = 0.0f;
	}
}

void renderGame(GameMap* map, Player player, Rectangle frameRect, Vector2 mousePos) {

	int playerTileY = player.position.y / 32;
	for (int i = 0; i < MAP_SIZE; i++) {
			for (int j = 0; j < MAP_SIZE; j++) {
				int row = map->Tiles[i][j] / 8;
				map->TileRect.x = (map->Tiles[i][j] - (row * 8)) * 32;
				map->TileRect.y = (map->Tiles[i][j] / 8) * 32;
				DrawTextureRec(map->TileTex, map->TileRect, (Vector2) { j * 32, i * 32 }, WHITE);
				if (map->Objects[i][j] > 2 && map->Objects[i][j] < 5) {
					setPropRect(map->Objects[i][j], &map->ObjRect);
					DrawTextureRec(map->PlantTex, map->ObjRect, (Vector2) { (j * 32), (i * 32)}, WHITE);
				}
			}
		}

		for (int i = 0; i < MAP_SIZE; i++) {
			for (int j = 0; j < MAP_SIZE; j++) {
				if (map->Objects[i][j] > 0) {
					if (map->Objects[i][j] == 1) {
						setPropRect(map->Objects[i][j], &map->ObjRect);
						DrawTextureRec(map->ObjectTex, map->ObjRect, (Vector2) { j * 32, (i * 32) - 32.0f }, WHITE);
					}
					else if (map->Objects[i][j] == 2) {
						setPropRect(map->Objects[i][j], &map->ObjRect);
						DrawTextureRec(map->PlantShadows, map->ObjRect, (Vector2) { (j * 32) - 64.0f
							, (i * 32) - 128.0f
						}, (Color) { 255, 255, 255, 100 });
						DrawTextureRec(map->PlantTex, map->ObjRect, (Vector2) { (j * 32) - 64.0f
							, (i * 32) - 128.0f }, WHITE);
					}
				}
			}
			if (i == playerTileY) {
				renderPlayer(&player, frameRect, mousePos);
			}
		}
}
void updateGame() {}

GameObject createGameObject(int type) {

	switch (type) {
	case GRAVE_CROSS:
		return (GameObject) {
			type,
				2,
				0,
				"Here lies so and so...\0",
				0,
				0
		};
		break;
	case TREE1:
		return (GameObject) {
			type,
				2,
				0,
				"Ah nature...\0",
				0,
				0
		};
		break;
	default:
		return (GameObject) {
			EMPTY,
				0, 0, "\0", 0, 0
		};
	}
}