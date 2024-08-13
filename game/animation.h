#pragma once
#include "raylib.h"

typedef struct AnimationFrames {
	int frameStart;
	int frameEnd;
	int rowOffset;
}AnimationFrames;

typedef struct Animation {
	Vector2 texSize;
	int frameSize;
	int currentFrame;
	AnimationFrames animations[16];
}Animation;
