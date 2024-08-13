#include "raylib.h"
#include "player.h"
#include "animation.h"
#include "raymath.h"
#include "math.h"

#define DIAGONAL_DAMPER 0.70710678118f
Player createPlayer(Vector2 position, Texture2D *body_tex, Texture2D *head_tex, Texture2D *cloak_tex, Texture2D *glove_tex) {
	Vector2 headOffset = (Vector2){ position.x, position.y - 10.0f };
	Vector2 cloakOffset = (Vector2){ position.x, position.y - 5.0f };
	Animation playerAnims;
	// Going to define the animations here for now
	AnimationFrames idleFrames = (AnimationFrames){
		0, 5, 0
	};
	AnimationFrames runFrames = (AnimationFrames){
		6, 10, 2
	};
	//
	playerAnims.texSize = (Vector2){ 96, 128 };
	playerAnims.frameSize = 32;
	playerAnims.currentFrame = 0;
	playerAnims.animations[IDLE] = idleFrames;
	playerAnims.animations[RUN] = runFrames;
	return (Player) {
		position,
		headOffset,
		cloakOffset,
		*body_tex,
		*head_tex,
		*cloak_tex,
		*glove_tex,
		playerAnims,
		IDLE,
		false
	};
}

void handlePlayerMovement(Player* player) {
	float speed = 1.8f;
	float mov_x = 0.0f;
	float mov_y = 0.0f;
	// this will change if we are moving diagonally
	float speed_mod = 1.0f;

	if (IsKeyDown(KEY_D)) {
		mov_x += speed;
	}
	if (IsKeyDown(KEY_A)) {
		mov_x -= speed;
	}
	if (IsKeyDown(KEY_W)) {
		mov_y -= speed;
	}
	if (IsKeyDown(KEY_S)) {
		mov_y += speed;
	}
	if (mov_x != 0 && mov_y != 0) {
		speed_mod = DIAGONAL_DAMPER;
	}
	mov_x *= speed_mod;
	mov_y *= speed_mod;

	player->position.x += mov_x;
	player->position.y += mov_y;

	// change player animation state
	if (mov_x == 0 && mov_y == 0) {
		if (player->animState != IDLE) {
			player->animState = IDLE;
			player->animation.currentFrame = player->animation.animations[player->animState].frameStart;
		}
	}
	else {
		if (player->animState != RUN) {
			player->animState = RUN;
			player->animation.currentFrame = player->animation.animations[player->animState].frameStart;
		}
	}
}

void renderPlayer(Player *player, Rectangle frameRec, Vector2 mousePos) {

	float deltaX = player->position.x - mousePos.x;
	float deltaY = player->position.y - mousePos.y;

	float angle = (float)(atan2(deltaY, deltaX) * 180.0f / PI) + 180;

	float xRotBuffer = 2.0f;
	if (mousePos.x < player->position.x) {
		player->isFlipped = true;
		angle = angle - 180;
		xRotBuffer = -2.0f;
	}
	else {
		player->isFlipped = false;
		angle = angle;
	}
	if (player->isFlipped) {
		frameRec.width = -frameRec.width;
	}


	DrawTextureRec(
		player->cloak_tex,
		frameRec,
		player->position,
		WHITE
			);

	DrawTextureRec(
		player->body_tex,
		frameRec,
		player->position,
		WHITE
	);

	player->headOffset.x = player->position.x;
	player->headOffset.y = player->position.y - 8.0f;

	DrawTexturePro(
		player->head_tex,
		frameRec,
		(Rectangle) { player->position.x + 16.0f, player->position.y + 10.0f, 32.0f, frameRec.height },
		(Vector2) { 16.0f, 20.0f },
		angle,
		WHITE
	);

	DrawTextureRec(
		player->glove_tex,
		frameRec,
		player->position,
		WHITE
	);

}
