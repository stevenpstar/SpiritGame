#include "ui.h"

void fillRowGrass(InventoryRow* row, Texture2D* tex) {
	// hard coding for now
	InvItem fourStones = (InvItem){
		32, *tex, (Rectangle) { 0.0f, 32.0f * 4.0f, 32.0f, 32.0f }
	};
	InvItem fourStonesDamaged = (InvItem){
		33, *tex, (Rectangle) { 32.0f, 32.0f * 4.0f, 32.0f, 32.0f }
	};
	InvItem twoStonesRight = (InvItem){
		34, *tex, (Rectangle) { 64.0f, 32.0f * 4.0f, 32.0f, 32.0f }
	};
	InvItem twoStonesLeft = (InvItem){
		35, *tex, (Rectangle) { 96.0f, 32.0f * 4.0f, 32.0f, 32.0f }
	};
	row->item[0] = fourStones;
	row->item[1] = fourStonesDamaged;
	row->item[2] = twoStonesRight;
	row->item[3] = twoStonesLeft;
	row->item[4] = fourStones;
	row->item[5] = fourStones;
	row->item[6] = fourStones;
	row->item[7] = fourStones;
	row->item[8] = fourStones;
}