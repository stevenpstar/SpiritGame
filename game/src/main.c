#include "player.h"
#include "raylib.h"
#include "math.h"
#include "ui.h"
#include "game.h"

#define PLAYER_FRAME_SIZE 32
#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

int generateWorld(int world[MAP_SIZE][MAP_SIZE], int objects[MAP_SIZE][MAP_SIZE]) {
	// generate world tiles randomly

	srand(time(NULL));
	for (int i = 0; i < MAP_SIZE; i++) {
		for (int j = 0; j < MAP_SIZE; j++) {
			int tile = (rand() % 32) + 0;
			world[i][j] = tile;
			if (tile < 30) {
				objects[i][j] = 0;
			}
			else {
				objects[i][j] = 2;
			}
		}
	}
}

void updateMiniMap(int miniMap[1024], int Tiles[32][32], int Objects[32][32]) {
	int indx = 0;
	for (int i = 31; i >= 0; i--) {
		for (int j = 0; j < 32; j++) {
			char buffer[5]; // convert index
			buffer[0] = '\0';
			sprintf(buffer, "%ld", indx);
			char indName[32] = "u_map[\0";
			int sI = 6;
			for (int z = 0; buffer[z] != '\0'; z++) {
				indName[sI] = buffer[z];
				sI++;
			}
			indName[sI] = ']';
			miniMap[indx] = Tiles[i][j];
			if (Objects[i][j] == 2) {
				miniMap[indx] = 50;
			}

			indx++;
		}
	}
}

int main(void) {
	const int screenWidth = 1920;
	const int screenHeight = 1080;

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
	Texture2D plants_tex = LoadTexture("resources/plants.png");
	Texture2D props_shadows_tex = LoadTexture("resources/objectshadows.png");
	Texture2D plants_shadows_tex = LoadTexture("resources/plantShadows.png");
	Texture2D walls_tex = LoadTexture("resources/walls.png");
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
	Shader miniMapShader = LoadShader(0, TextFormat("resources/shaders/miniMapShader.fs", GLSL_VERSION));
	RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
	RenderTexture2D miniMap = LoadRenderTexture(32.0f, 32.0f);
	int resVarLoc = GetShaderLocation(lightingShader, "u_resolution");
	int maxAlphaLoc = GetShaderLocation(lightingShader, "max_alpha");
	float resVar[2] = { (float)screenWidth, (float)screenHeight };

	int colorLoc = GetShaderLocation(lightingShader, "u_colour");
	float colorVar[3] = {0.0f, 0.0f, 0.0f};

	int playerVarLoc = GetShaderLocation(lightingShader, "player_pos");
	int mouseVarLoc = GetShaderLocation(lightingShader, "u_mouse");

	// world obj (temporary)
	int grassTiles[MAP_SIZE][MAP_SIZE];
	int worldProps[MAP_SIZE][MAP_SIZE];
	//generateWorld(grassTiles);

	Vector2 position = { 150.0f, 280.0f };
	Player player = createPlayer(position, &body_tex, &head_tex, &cloak_tex, &glove_tex, &shadow_tex);
	Rectangle frameRec = { 0.0f, 0.0f, (float)body_tex.width / 3, (float)body_tex.height / 4 };
	Rectangle grassFrameRec = { 0.0f, 0.0f, 32.0f, 32.0f };
	int currentFrame = 0;

	// Camera settings

	Camera2D camera = { 0 };
	camera.target = (Vector2) { position.x + 20.0f, position.y + 20.0f };
	camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;

	GameMap DefaultMap = (GameMap){
		grassMap,
		grassFrameRec,
		props_tex,
		props_shadows_tex,
		plants_tex,
		plants_shadows_tex,
		walls_tex,
		propsRect,
		camera,
		grassTiles,
		worldProps
	};
	
	generateWorld(DefaultMap.Tiles, DefaultMap.Objects);
	// scuffed and unoptimised
//	int miniMapLoc[1024];


	int playerPosLoc = GetShaderLocation(miniMapShader, "pp");
	int miniMapLoc = GetShaderLocation(miniMapShader, "u_map");
	int miniMapVal[1024];
	int indx = 0;
	updateMiniMap(miniMapVal, DefaultMap.Tiles, DefaultMap.Objects);

	SetShaderValueV(miniMapShader, miniMapLoc, miniMapVal, SHADER_UNIFORM_INT, 1024);

	if (playerPosLoc == -1) {
		printf("Player Pos Loc Not found\n");
	}

	Vector2 playerScreenPos = GetWorldToScreen2D(player.position, camera);
	float playerVar[2] = { playerScreenPos.x, abs(playerScreenPos.y - screenHeight) };
	float mouseVar[2] = { playerScreenPos.x, abs(playerScreenPos.y - screenHeight) };
	float maxAlpha = 0.0f;

	Vector2 cameraMin = (Vector2){ 0 + ((screenWidth / 2) / camera.zoom), 0 + ((screenHeight / 2) / camera.zoom) };
	Vector2 cameraMax = (Vector2){ (MAP_SIZE * MAP_SIZE) - ((screenWidth / 2) / camera.zoom), (MAP_SIZE * MAP_SIZE) - ((screenHeight / 2) / camera.zoom)};

	int framesCounter = 0;
	int framesSpeed = 12;
	int rowOffset = 2;
	int frameRow = 0 + rowOffset;
	int selectFrame = 0;

	int selectTileX = 0;
	int selectTileY = 0;

	bool select = true;
	int selectSoundDelay = 16;
	int selectDelayCounter = 0;
	bool tileSelectChange = false;

	bool playSmoke = false;
	int smokeFrame = 0;
	Vector2 particlePos = (Vector2){ 0.0f, 0.0f };

	Vector2 mousePos;
	Vector2 cameraPos = (Vector2){ player.position.x - screenWidth / 2.0f,
		player.position.y - screenHeight / 2.0f };

	// colour targets for shader
	float sunsetR = 0.69f;
	float sunsetG = 0.298f;
	float sunsetB = 0.047f;

	float nightR = 0.078f;
	float nightG = 0.075f;
	float nightB = 0.188f;

	float colourChange = 0.01f;
	int playerTiles[2] = { 0, 0 };

	InventoryRow invRows[3];
	fillRowGrass(&invRows[0], &grassMap);
	int selectedRow = 0;
	int slotSelected = 0;
	int tileId = 32;
	int dayTimer = 0;
	bool sunSetting = true;

	SetTargetFPS(60);
	HideCursor();
	while (!WindowShouldClose()) {
		playerScreenPos = GetWorldToScreen2D(player.position, camera);

		playerTiles[0] = (int)player.position.x / 32.0f;
		playerTiles[1] = (int)player.position.y / 32.0f;

		playerVar[0] = playerScreenPos.x + 32.0f;
		playerVar[1] = abs(playerScreenPos.y - screenHeight) - 32.0f;

		mouseVar[0] = -256.0f;// GetMousePosition().x + 32.0f;
		mouseVar[1] = -256.0f;// abs(GetMousePosition().y - screenHeight) - 32.0f;

		SetShaderValue(lightingShader, resVarLoc, resVar, SHADER_UNIFORM_VEC2);
		SetShaderValue(lightingShader, playerVarLoc, playerVar, SHADER_UNIFORM_VEC2);
		SetShaderValue(lightingShader, mouseVarLoc, mouseVar, SHADER_UNIFORM_VEC2);
		SetShaderValue(lightingShader, maxAlphaLoc, &maxAlpha, SHADER_UNIFORM_FLOAT);
		SetShaderValue(lightingShader, colorLoc, colorVar, SHADER_UNIFORM_VEC3);


		//SetShaderValueV(miniMapShader, miniMapLoc, &miniMapVal, SHADER_UNIFORM_INT, 1024);
		// Mouse position
		mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
		selectTileX = mousePos.x / 32;
		selectTileY = mousePos.y / 32;
		// Update and Input
		handlePlayerMovement(&player, DefaultMap.Objects);
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

		// temp position for changing selected inv slot
		if (IsKeyPressed(KEY_ONE)) {
			slotSelected = 0;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_TWO)) {
			slotSelected = 1;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_THREE)) {
			slotSelected = 2;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_FOUR)) {
			slotSelected = 3;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_FIVE)) {
			slotSelected = 4;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_SIX)) {
			slotSelected = 5;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_SEVEN)) {
			slotSelected = 6;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_EIGHT)) {
			slotSelected = 7;
			PlaySound(click3Ogg);
		}
		else if (IsKeyPressed(KEY_NINE)) {
			slotSelected = 8;
			PlaySound(click3Ogg);
		}

		tileId = invRows[selectedRow].item[slotSelected].ItemId;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (!select) { select = true; }
			if (!playSmoke) { playSmoke = true; }
			selectFrame = 0;
			smokeFrame = 0;
			PlaySound(click2Ogg);
			trauma = 0.1f;
			if (!placingProp) {
				DefaultMap.Tiles[selectTileY][selectTileX] = tileId;
			}
			else {
				DefaultMap.Objects[selectTileY][selectTileX] = GRAVE_CROSS;
			}
			particlePos.x = (selectTileX * 32.0f) - 16.0f;
			particlePos.y = (selectTileY * 32.0f) - 16.0f;
			updateMiniMap(miniMapVal, DefaultMap.Tiles, DefaultMap.Objects);
			SetShaderValueV(miniMapShader, miniMapLoc, miniMapVal, SHADER_UNIFORM_INT, 1024);
		}
		else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			if (!select) { select = true; }
			if (!playSmoke) { playSmoke = true;  }
			selectFrame = 0;
			smokeFrame = 0;
			PlaySound(click2Ogg);
			trauma = 0.1f;
			if (!placingProp) {
				DefaultMap.Tiles[selectTileY][selectTileX] = 0;
			}
			else {
				DefaultMap.Objects[selectTileY][selectTileX] = EMPTY;
			}
			particlePos.x = (selectTileX * 32.0f) - 16.0f;
			particlePos.y = (selectTileY * 32.0f) - 16.0f;
			updateMiniMap(miniMapVal, DefaultMap.Tiles, DefaultMap.Objects);
			SetShaderValueV(miniMapShader, miniMapLoc, miniMapVal, SHADER_UNIFORM_INT, 1024);
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
			if (sunSetting) {
				dayTimer+=1;
				if (dayTimer > 1000) {
					sunSetting = false;
				}
			}
			else {
				dayTimer-=1;
				if (dayTimer < 0) {
					sunSetting = true;
				}
			}

			// changing colour works
			if (dayTimer > 500 && dayTimer < 700) {
				if (colorVar[0] < sunsetR) {
					colorVar[0] += 0.001f;
				}
				else if(colorVar[1] > sunsetR) {
					colorVar[0] -= 0.001f;
				}
				if (colorVar[1] < sunsetG) {
					colorVar[1] += 0.001f;
				}
				else if (colorVar[1] > sunsetG) {
					colorVar[1] -= 0.001f;
				}
				if (colorVar[2] < sunsetB) {
					colorVar[2] += 0.001f;
				}
				else if (colorVar[2] > sunsetB) {
					colorVar[2] -= 0.001f;
				}
			}
			else if (dayTimer > 700) {
				if (colorVar[0] < nightR) {
					colorVar[0] += 0.001f;
				}
				else if (colorVar[0] > nightR) {
					colorVar[0] -= 0.001f;
				}
				if (colorVar[1] < nightG) {
					colorVar[1] += 0.001f;
				}
				else if (colorVar[1] > nightG) {
					colorVar[1] -= 0.001f;
				}
				if (colorVar[2] < nightB) {
					colorVar[2] += 0.001f;
				}
				else if (colorVar[2] > nightB) {
					colorVar[2] -= 0.001f;
				}
			}
			if (dayTimer >= 500) {
				maxAlpha = ((float)dayTimer - 500.0f) / 500.0f;
			}
			else {
				colorVar[0] = 0.0f;
				colorVar[1] = 0.0f;
				colorVar[2] = 0.0f;
			}
			framesCounter = 0;
			player.animation.currentFrame++;
			//if (select)
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
			//	select = false;
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
		renderGame(&DefaultMap , player, frameRec, mousePos);
		// render select
		if (DefaultMap.Objects[selectTileY][selectTileX] != EMPTY) {
			DrawTextureRec(select_tex, selectRect, (Vector2) { selectTileX * 32, selectTileY * 32 }, WHITE);
		}
		else {
			DrawRectangle(selectTileX * 32, selectTileY * 32, 32.0f, 32.0f, (Color) { 255, 255, 255, 50 });
		}
		// render player

		if (playSmoke) {
			DrawTextureRec(particles_tex, smokeRect, particlePos, WHITE);
		}
		// debug player tile
	//	int px = (player.position.x + 16) / 32;
	//	int py = (player.position.y + 32) / 32;
	//	DrawRectangle(px * 32, py * 32, 32, 32, (Color){255, 0, 0, 100});
		EndMode2D();
		DrawRectangle(0, 0, screenWidth, screenHeight, (Color) { 0, 0, 0, 100 });

		BeginShaderMode(lightingShader);
		DrawTextureRec(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);

		EndShaderMode();
		BeginShaderMode(miniMapShader);
		DrawTexturePro(miniMap.texture, 
			(Rectangle) { 0.0f, 0.0f, (float)miniMap.texture.width, (float)-miniMap.texture.height },
			(Rectangle) { 0.0f, 824.0f, (float)miniMap.texture.width * 8, (float)miniMap.texture.height * 8}, 
			(Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);

		EndShaderMode();
		DrawRectangle(playerTiles[0] * 8, (playerTiles[1] * 8) + 824.0f, 8, 8, WHITE);

	//	DrawText(TextFormat("DayTimer: %d", dayTimer), 190, 200, 20, LIGHTGRAY);
		// UI Drawing
		// hard coding slots for now

		float xOffset = (screenWidth - (128.0f * 9)) / 2;
		for (int i = 0; i < 9; i++) {
			float size = 96.0f;
			float lineWidth = 4.0f;
			Color c = (Color){ 0, 0, 0, 255 };
			if (slotSelected == i) {
				size = 96.0f;
				lineWidth = 8.0f;
				c = (Color){ 255, 255, 255, 255 };
			}

			DrawTexturePro(invRows[selectedRow].item[i].tex,
				invRows[selectedRow].item[i].rect,
				(Rectangle) {
				xOffset + (128.0f * i), screenHeight - 128.0f, 96.0f, 96.0f
			},
				(Vector2) {
				0.0f, 0.0f
			}, 0.0f, WHITE);

			DrawRectangleLinesEx((Rectangle) { xOffset + (128.0f * i), screenHeight - 128.0f, size, size }, lineWidth, c);
		}

		DrawTextureRec(cursor_tex, cursorRect, GetMousePosition(), WHITE);

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
	UnloadTexture(plants_tex);
	UnloadTexture(plants_shadows_tex);
	UnloadTexture(props_shadows_tex);
	UnloadTexture(walls_tex);
	UnloadRenderTexture(target);
	UnloadRenderTexture(miniMap);
	// Unload SFX
	UnloadSound(click1Ogg);
	UnloadSound(click2Ogg);
	UnloadSound(click3Ogg);
	UnloadSound(click4Ogg);
	UnloadSound(click5Ogg);

	// Unload VFX
	UnloadShader(lightingShader);
	UnloadShader(miniMapShader);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}


