#include "player.h"
#include "raylib.h"
#include "math.h"

#define PLAYER_FRAME_SIZE 32
#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

enum ShrineObjects {
	EMPTY,
	GRAVE_CROSS
};

void setPropRect(int propId, Rectangle* propsRect) {
	switch (propId) {
	case GRAVE_CROSS:
		//224 288
		propsRect->x = 224.0f;
		propsRect->y = 288.0f;
		break;
	default:
		propsRect->x = 0.0f;
		propsRect->y = 0.0f;
	}
}

void generateWorld(int world[32][32]) {
	// generate world tiles randomly
	srand(time(NULL));
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			int tile = (rand() % 32) + 0;
			world[i][j] = tile;
		}
	}
}

int main(void) {
	const int screenWidth = 1920;
	const int screenHeight = 1080;
	bool isRunning = false;
	int idleFrames[] = { 0, 1, 2, 3, 4, 5 };
	int runFrames[] = { 6, 7, 8, 9, 10 };

	// TODO: Temporary variable for testing
	bool placingProp = false;

	// 0.0f = 1.0f
	float trauma = 0.0f;

	float xShakeDist = 4.0f;
	bool shakeRight = true;

	InitWindow(screenWidth, screenHeight, "Paladin's Spirit");
	InitAudioDevice();

	//World Textures
	Texture2D grassMap = LoadTexture("resources/grasstiles.png");

	// Character textures
	Texture2D body_tex = LoadTexture("resources/body.png");
	Texture2D head_tex = LoadTexture("resources/head.png");
	Texture2D cloak_tex = LoadTexture("resources/cloak.png");
	Texture2D glove_tex = LoadTexture("resources/glove.png");
	Texture2D shadow_tex = LoadTexture("resources/shadowsprite.png");
	Rectangle shadowRect = { 0.0f, 0.0f, 32.0f, 32.0f };

	Texture2D particles_tex = LoadTexture("resources/particles1.png");
	Rectangle smokeRect = { 0.0f, 384.0f, 64.0f, 64.0f };

	Texture2D props_tex = LoadTexture("resources/props.png");
	Rectangle propsRect = { 0.0f, 0.0f, 64.0f, 64.0f };

	// UI textures
	Texture2D select_tex = LoadTexture("resources/select.png");
	Rectangle selectRect = { 0.0f, 0.0f, 32.0f, 32.0f };

	Texture2D cursor_tex = LoadTexture("resources/cursor.png");
	Rectangle cursorRect = { 0.0f, 0.0f, 32.0f, 32.0f };
	Rectangle cursorScaled = { 0.0f, 0.0f, 8.0f, 8.0f };

	// Sounds
	Sound click1Ogg = LoadSound("resources/click_001.ogg");
	Sound click2Ogg = LoadSound("resources/click_002.ogg");
	Sound click3Ogg = LoadSound("resources/click_003.ogg");
	SetSoundVolume(click3Ogg, 0.2f);
	Sound click4Ogg = LoadSound("resources/click_004.ogg");
	Sound click5Ogg = LoadSound("resources/click_005.ogg");

	// Shaders
	Shader lightingShader = LoadShader(0, TextFormat("resources/shaders/lighting.fs", GLSL_VERSION));
	RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
	int resVarLoc = GetShaderLocation(lightingShader, "u_resolution");
	float resVar[2] = { (float)screenWidth, (float)screenHeight };

	int playerVarLoc = GetShaderLocation(lightingShader, "player_pos");
	int mouseVarLoc = GetShaderLocation(lightingShader, "u_mouse");

	// world obj (temporary)
	int grassTiles[32][32];
	int worldProps[32][32];
	generateWorld(grassTiles);

	Vector2 position = { 150.0f, 280.0f };
	Player player = createPlayer(position, &body_tex, &head_tex, &cloak_tex, &glove_tex);
	Rectangle frameRec = { 0.0f, 0.0f, (float)body_tex.width / 3, (float)body_tex.height / 4 };
	Rectangle grassFrameRec = { 0.0f, 0.0f, 32.0f, 32.0f };
	int currentFrame = 0;

	// Camera settings

	Camera2D camera = { 0 };
	camera.target = (Vector2) { position.x + 20.0f, position.y + 20.0f };
	camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;

	Vector2 playerScreenPos = GetWorldToScreen2D(player.position, camera);
	float playerVar[2] = { playerScreenPos.x, abs(playerScreenPos.y - screenHeight) };
	float mouseVar[2] = { playerScreenPos.x, abs(playerScreenPos.y - screenHeight) };

	Vector2 cameraMin = (Vector2){ 0 + ((screenWidth / 2) / camera.zoom), 0 + ((screenHeight / 2) / camera.zoom) };
	Vector2 cameraMax = (Vector2){ (32 * 32) - ((screenWidth / 2) / camera.zoom), (32 * 32) - ((screenHeight / 2) / camera.zoom)};

	int framesCounter = 0;
	int framesSpeed = 12;
	int rowOffset = 2;
	int frameRow = 0 + rowOffset;
	int selectFrame = 0;

	int selectTileX = 0;
	int selectTileY = 0;

	bool select = false;
	int selectSoundDelay = 16;
	int selectDelayCounter = 0;
	bool tileSelectChange = false;

	bool playSmoke = false;
	int smokeFrame = 0;
	Vector2 particlePos = (Vector2){ 0.0f, 0.0f };

	Vector2 mousePos;
	Vector2 cameraPos = (Vector2){ player.position.x - screenWidth / 2.0f,
		player.position.y - screenHeight / 2.0f };


	SetTargetFPS(60);
	HideCursor();
	while (!WindowShouldClose()) {
		playerScreenPos = GetWorldToScreen2D(player.position, camera);
		playerVar[0] = playerScreenPos.x + 32.0f;
		playerVar[1] = abs(playerScreenPos.y - screenHeight) - 32.0f;

		mouseVar[0] = GetMousePosition().x + 32.0f;
		mouseVar[1] = abs(GetMousePosition().y - screenHeight) - 32.0f;

		SetShaderValue(lightingShader, resVarLoc, resVar, SHADER_UNIFORM_VEC2);
		SetShaderValue(lightingShader, playerVarLoc, playerVar, SHADER_UNIFORM_VEC2);
		SetShaderValue(lightingShader, mouseVarLoc, mouseVar, SHADER_UNIFORM_VEC2);

		// Mouse position
		mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
		if ((int)mousePos.x / 32 != selectTileX || (int)mousePos.y / 32 != selectTileY) {
			if (!tileSelectChange) {
				PlaySound(click3Ogg);
			}
			tileSelectChange = true;
		}
		if (tileSelectChange) {
			selectDelayCounter++;
			if (selectDelayCounter > (60 / selectSoundDelay)) {
				tileSelectChange = false;
				selectDelayCounter = 0;
			}
		}
		selectTileX = mousePos.x / 32;
		selectTileY = mousePos.y / 32;
		// Update and Input
		handlePlayerMovement(&player);
		camera.target = (Vector2){ player.position.x, player.position.y };
		if (camera.target.x < cameraMin.x) {
			camera.target.x = cameraMin.x;
		}
		else if (camera.target.x > cameraMax.x) {
			camera.target.x = cameraMax.x;
		}
		if (camera.target.y < cameraMin.y) {
			camera.target.y = cameraMin.y;
		}
		else if (camera.target.y > cameraMax.y) {
			camera.target.y = cameraMax.y;
		}
		camera.target.x += trauma * powf(xShakeDist, 2.0f);
		camera.target.y += trauma * powf(xShakeDist, 2.0f);
		cameraPos.x = player.position.x - (screenWidth / 2.0f);
		cameraPos.y = player.position.y - (screenHeight / 2.0f);
		camera.rotation = trauma * powf(xShakeDist, 2.0f);

		float initCamTarget = camera.target.x;

		if (shakeRight) {
			shakeRight = false;
			xShakeDist = -2.0f;
		}
		else {
			shakeRight = true;
			xShakeDist = 1.0f;
		}

		if (IsKeyPressed(KEY_TAB)) {
			placingProp = !placingProp;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (!select) { select = true; }
			if (!playSmoke) { playSmoke = true; }
			int stoneTile = 32;
			int randTile = (rand() % 4);
			switch (randTile) {
			case 0:
				stoneTile = 32;
				break;
			case 1:
				stoneTile = 33;
				break;
			case 2:
				stoneTile = 40;
				break;
			case 3:
				stoneTile = 41;
				break;
			default:
				stoneTile = 32;
			}
			selectFrame = 0;
			smokeFrame = 0;
			PlaySound(click2Ogg);
			trauma = 0.1f;
			if (!placingProp) {
				grassTiles[selectTileY][selectTileX] = stoneTile;
			}
			else {
				worldProps[selectTileY][selectTileX] = GRAVE_CROSS;
			}
			particlePos.x = (selectTileX * 32.0f) - 16.0f;
			particlePos.y = (selectTileY * 32.0f) - 16.0f;
		}
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			if (!select) { select = true; }
			if (!playSmoke) { playSmoke = true;  }
			selectFrame = 0;
			smokeFrame = 0;
			PlaySound(click2Ogg);
			trauma = 0.1f;
			if (!placingProp) {
				grassTiles[selectTileY][selectTileX] = 0;
			}
			else {
				worldProps[selectTileY][selectTileX] = EMPTY;
			}
			particlePos.x = (selectTileX * 32.0f) - 16.0f;
			particlePos.y = (selectTileY * 32.0f) - 16.0f;
		}

		if (trauma > 0.0f) {
				trauma -= 0.01f;
				if (trauma < 0.0f) {
					trauma = 0.0f;
				}
			}


		//Animations
		framesCounter++;
		if (framesCounter >= (60 / framesSpeed)) {

		
			framesCounter = 0;
			player.animation.currentFrame++;
			if (select)
				selectFrame++;

			if (playSmoke)
				smokeFrame++;

			frameRow = player.animation.currentFrame / 3;
			int lastFrame = player.animation.animations[player.animState].frameEnd;
			if (player.animation.currentFrame > lastFrame) {
				player.animation.currentFrame = player.animation.animations[player.animState].frameStart;
				frameRow = player.animation.animations[player.animState].rowOffset;
			}
			if (selectFrame > 3 || select == false) {
				selectFrame = 0;
				select = false;
			}

			if (smokeFrame > 10 || !playSmoke) {
				smokeFrame = 0;
				playSmoke = false;
			}
			frameRec.x = (float)(player.animation.currentFrame - (frameRow * 3)) * (float)body_tex.width / 3;
			frameRec.y = (float)frameRow * (float)body_tex.height / 4;

			// handling select animation here too for now
			selectRect.x = (float)selectFrame * 32.0f;

			// and particles
			smokeRect.x = (float)smokeFrame * 64.0f;
		}

		// Render
		BeginTextureMode(target);
		ClearBackground(RAYWHITE);
		EndTextureMode();
		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode2D(camera);
		// render world
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				int row = grassTiles[i][j] / 8;
				grassFrameRec.x = (grassTiles[i][j] - (row * 8)) * 32;
				grassFrameRec.y = (grassTiles[i][j] / 8) * 32;
				DrawTextureRec(grassMap, grassFrameRec, (Vector2) { j * 32, i * 32 }, WHITE);
			}
		}

		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				if (worldProps[i][j] > EMPTY) {
					setPropRect(worldProps[i][j], &propsRect);
					DrawTextureRec(props_tex, propsRect, (Vector2) { j * 32, (i * 32) - 32.0f }, WHITE);
				}
			}
		}

		// render select
		DrawTextureRec(select_tex, selectRect, (Vector2) { selectTileX * 32, selectTileY * 32 }, WHITE);
		// render player
		DrawTextureRec(shadow_tex, shadowRect, (Vector2) { player.position.x, player.position.y + 16.0f }, WHITE);
		renderPlayer(&player, frameRec, mousePos);
		if (playSmoke) {
			DrawTextureRec(particles_tex, smokeRect, particlePos, WHITE);
		}
		// render cursor
		cursorScaled.x = mousePos.x;
		cursorScaled.y = mousePos.y;
		DrawTexturePro(cursor_tex, cursorRect, cursorScaled, (Vector2) {0.0f, 0.0f }, 1.0f, WHITE);
		EndMode2D();
		DrawRectangle(0, 0, screenWidth, screenHeight, (Color) { 0, 0, 0, 100 });
		DrawText("Text!", 190, 200, 20, LIGHTGRAY);

		BeginShaderMode(lightingShader);
		DrawTextureRec(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);
		EndShaderMode();

		EndDrawing();
	}
	UnloadTexture(body_tex);
	UnloadTexture(head_tex);
	UnloadTexture(cloak_tex);
	UnloadTexture(glove_tex);
	UnloadTexture(shadow_tex);
	UnloadTexture(particles_tex);
	UnloadTexture(grassMap);
	UnloadTexture(select_tex);
	UnloadTexture(cursor_tex);
	UnloadTexture(props_tex);
	UnloadRenderTexture(target);
	// Unload SFX
	UnloadSound(click1Ogg);
	UnloadSound(click2Ogg);
	UnloadSound(click3Ogg);
	UnloadSound(click4Ogg);
	UnloadSound(click5Ogg);

	// Unload VFX
	UnloadShader(lightingShader);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}


