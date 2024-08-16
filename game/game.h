#pragma once
#include "ui.h"
#include "player.h"
#include "raylib.h"

typedef struct GameMap {
	Texture2D TileTex;
	Rectangle TileRect;
	Texture2D ObjectTex;
	Texture2D ObjectShadows;
	Texture2D PlantTex;
	Texture2D PlantShadows;
	Texture2D WallsTex;
	Rectangle ObjRect;
	Camera2D Camera;
	int Tiles[32][32];
	int Objects[32][32];
}GameMap;

enum ShrineObjects {
	EMPTY,
	GRAVE_CROSS,
	TREE1,
	GRASS1,
	GRASS2,
	GRASS3
};

typedef struct GameObject {
	int Type;
	int Health;
	int Damage;
	char Desc[128];
	int ActionDelay;
	int ActionTime;
}GameObject;

GameObject createGameObject(int type);
void renderGame(GameMap* map, Player player, Rectangle frameRect, Vector2 mousePos);
