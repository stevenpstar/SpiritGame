#pragma once
#include "raylib.h"

#define MAP_SIZE 32
typedef struct InvItem {
	int ItemId;
	Texture2D tex;
	Rectangle rect;
}InvItem;

typedef struct InvSlot {
	InvItem item;
}InvSlot;

typedef struct InventoryRow {
	InvItem item[9];
}InventoryRow;

void fillRowGrass(InventoryRow* row, Texture2D* tex);
