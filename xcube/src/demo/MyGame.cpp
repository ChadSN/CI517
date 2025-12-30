#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), numAmmo(5), numHealth(3), gameWon(false) {
	srand((unsigned int)time(nullptr));																// Seed random number generator
	TTF_Font* font = ResourceManager::loadFont("res/fonts/arial.ttf", 24);							// Load font
	gfx->useFont(font);																				// Use font
	gfx->setVerticalSync(true);																		// Enable VSync
	loadResources();																				// Load resources
	loadMap();																						// Load map
}

void MyGame::handleKeyEvents() {
	if (!mySystem) return;																			// safety check
	if (playerEntityId < 0) return;																	// player not spawned
	float x = 0.0f, y = 0.0f;																		// input axes
	if (eventSystem->isPressed(Key::A)) { x -= 1.0f; }												// left
	if (eventSystem->isPressed(Key::D)) { x += 1.0f; }												// right
	if (eventSystem->isPressed(Key::W)) { y -= 1.0f; }												// up
	if (eventSystem->isPressed(Key::S)) { y += 1.0f; }												// down
	mySystem->setEntityInput(playerEntityId, x, y);													// set player input
}

void MyGame::update() {
	mySystem->update(FIXED_DT, playerEntityId);														// movement system
}

void MyGame::render() {
	mySystem->render(gfx);																			// render world
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_AQUA);																// set color
	std::string scoreStr = "SCORE: " + std::to_string(score);										// score string
	gfx->drawText(scoreStr, WINDOW_WIDTH - static_cast<int>(scoreStr.length()) * 24, 25);			// draw score right-aligned
	gfx->setDrawColor(SDL_COLOR_GREEN);																// set color
	std::string healthStr = "HEALTH: " + std::to_string(mySystem->getEntityHealth(playerEntityId));	// health string
	gfx->drawText(healthStr, 24, 25);																// draw
	if (gameWon) gfx->drawText("YOU WON", 250, 500);												// draw win message
}

void MyGame::loadResources() {
	// PLAYER SPRITES
	mySystem->loadSprite("player_idle_down", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 0);
	mySystem->loadSprite("player_idle_right", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 3);
	mySystem->loadSprite("player_idle_up", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 6);
	mySystem->loadSprite("player_walk_down", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 0);
	mySystem->loadSprite("player_walk_right", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 3);
	mySystem->loadSprite("player_walk_up", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 6);
	mySystem->loadSprite("player_death_down", "res/player_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 0);
	mySystem->loadSprite("player_death_right", "res/player_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 3);
	mySystem->loadSprite("player_death_up", "res/player_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 6);
	mySystem->loadSprite("player_shoot", "res/player_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 0);
	// NPC SPRITES
	mySystem->loadSprite("zombie_idle_down", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 0);
	mySystem->loadSprite("zombie_idle_right", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 3);
	mySystem->loadSprite("zombie_idle_up", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 1, 6);
	mySystem->loadSprite("zombie_walk_down", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 0);
	mySystem->loadSprite("zombie_walk_right", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 3);
	mySystem->loadSprite("zombie_walk_up", "res/zombie_walk_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 6);
	mySystem->loadSprite("zombie_death_down", "res/zombie_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 0);
	mySystem->loadSprite("zombie_death_right", "res/zombie_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 3);
	mySystem->loadSprite("zombie_death_up", "res/zombie_death_sheet.png", DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT, 3, 6);
	// GROUND SPRITES
	mySystem->loadSprite("ground", "res/ground.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("ground2", "res/ground2.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("ground3", "res/ground3.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("ground4", "res/ground4.png", TILE_SIZE, TILE_SIZE, 1, 0);
	// ROAD SPRITES
	mySystem->loadSprite("road", "res/road.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("roadH", "res/roadH.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("roadV", "res/roadV.png", TILE_SIZE, TILE_SIZE, 1, 0);
	// BLOCK SPRITE
	mySystem->loadSprite("block", "res/wall.png", TILE_SIZE, TILE_SIZE, 1, 0);

	// LOAD SOUNDS
	mySystem->loadSound("shoot", "res/sfx/shoot.wav");
	mySystem->loadSound("zombie_hit_sound", "res/sfx/zombie_hit.wav");
	mySystem->loadSound("player_hit_sound", "res/sfx/player_death.wav");
	mySystem->loadSound("heal_sound", "res/sfx/heal_sound.wav");
	mySystem->loadSound("ammo_sound", "res/sfx/ammo.wav");
}

void MyGame::loadMap() {
	for (int row = 0; row < LEVEL_ROWS; ++row) {													// FOR EACH ROW
		for (int col = 0; col < LEVEL_COLS; ++col) {												// FOR EACH COLUMN
			int posX = col * TILE_SIZE;																// position x
			int posY = row * TILE_SIZE;																// position y
			int groundTileType = getRandom(1, 5);													// random ground tile type
			std::string groundAnim = "ground";														// ground animation
			switch (groundTileType) {																// Set ground animation based on random type
			case 1: groundAnim = "ground"; break;
			case 2: groundAnim = "ground2"; break;
			case 3: groundAnim = "ground3"; break;
			case 4: groundAnim = "ground4"; break;
			default: groundAnim = "ground"; break;
			}
			mySystem->addGroundTile(groundAnim, posX, posY);										// add ground tile
			int slotContent = levelmap.map[0][row][col];											// get slot content
			switch (slotContent) {																	// SWITCH BASED ON SLOT CONTENT
			case 1: {
				int id = mySystem->spawnBlock(posX, posY);											// spawn block
				blockIds.push_back(id);																// store block ID
				break;
			}
			case 2: {
				playerEntityId = mySystem->spawnPC(posX, posY);										// spawn player
				break;
			}
			case 3: {
				auto id = mySystem->spawnNPC(posX, posY);											// spawn NPC
				npcIds.push_back(id);																// store NPC ID
				break;
			}
			case 4: {
				mySystem->addGroundTile("road", posX, posY);										// add road tile
				break;
			}
			case 5: {
				mySystem->addGroundTile("roadH", posX, posY);										// add horizontal road tile
				break;
			}
			case 6: {
				mySystem->addGroundTile("roadV", posX, posY);										// add vertical road tile
				break;
			}
			default:
				break;
			}
		}
	}
}