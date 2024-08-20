#pragma once
#include "raylib.h"
#include "animation.h"
enum PlayerAnimationState {IDLE, RUN};
typedef struct Player {
	Vector2 position;
	Vector2 headOffset;
	Vector2 cloakOffset;
	Texture2D body_tex;
	Texture2D head_tex;
	Texture2D cloak_tex;
	Texture2D glove_tex;
	Texture2D shadow_tex;
	Texture2D bow_tex;
	Animation animation;
	int animState;
	bool isFlipped;
}Player;

Player createPlayer(Vector2 position, 
	Texture2D* body,
	Texture2D* helm,
	Texture2D* cloak,
	Texture2D* glove,
	Texture2D *shadow_tex,
	Texture2D *bow_tex);
void renderPlayer(Player *player, Rectangle frameRec, Vector2 mousePos);
void handlePlayerMovement(Player* player, int Objects[32][32]);
