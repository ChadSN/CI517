#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), numAmmo(5), numHealth(3), gameWon(false) {
	srand((unsigned int)time(nullptr));																				// Seed random number generator
	font = ResourceManager::loadFont("res/fonts/arial.ttf", DEFAULT_FONT_SIZE);										// Load font
	gfx->useFont(font);																								// Use font
	gfx->setVerticalSync(true);																						// Enable VSync
	gfx->setWindowFocus();																							// Set window focus
	gfx->setWindowResizable();																						// Make window resizable
	gfx->setWindowTitle("CI517 - Chad Nippard");																	// Set window title
	mySystem->setWorldDimensions(worldWidth, worldHeight);															// Set world dimensions
	mySystem->setLevelsCount(LEVELS_COUNT); 																		// Set levels count
	loadResources();																								// Load resources
	loadMap();																										// Load map
}

void MyGame::handleKeyEvents() {
	if (!mySystem) return;																							// safety check
	if (playerEntityId < 0) return;																					// player not spawned
	float x = 0.0f, y = 0.0f;																						// input axes
	if (eventSystem->isPressed(Key::A)) { x -= 1.0f; }																// left
	if (eventSystem->isPressed(Key::D)) { x += 1.0f; }																// right
	if (eventSystem->isPressed(Key::W)) { y -= 1.0f; }																// up
	if (eventSystem->isPressed(Key::S)) { y += 1.0f; }																// down
	mySystem->setEntityInput(playerEntityId, x, y);																	// set player input
}

void MyGame::onLeftMouseButton() {
	if (!mySystem) return;																							// safety check
	if (playerEntityId < 0) return;																					// player not spawned
	if (mousePressed) return;																						// already pressed
	mousePressed = true;																							// set pressed
	Vector2i mouseScreenPos = Vector2i(eventSystem->getMousePos().x, eventSystem->getMousePos().y);					// get mouse screen pos
	Vector2f cameraPos = mySystem->getCameraPosition();																// get camera pos
	Vector2f mouseWorldPos = Vector2f(float(mouseScreenPos.x) + cameraPos.x, float(mouseScreenPos.y) + cameraPos.y);// convert to world pos
	Vector2f playerPos = mySystem->getEntityPosition(playerEntityId);												// get player pos
	mySystem->fireProjectile(playerEntityId, playerPos, mouseWorldPos);												// player shoot
}

void MyGame::update() {
	if (mySystem->isGameCompleted()) gameWon = true;																// check win condition
	if (eventSystem && !eventSystem->isPressed(Mouse::BTN_LEFT)) mousePressed = false;								// reset mouse pressed state
	mySystem->update(deltaTime, playerEntityId);																		// movement system
	if (mySystem->isLevelChanging()) {																				// IF LEVEL CHANGING
		blockIds.clear();																							// clear block IDs
		loadMap();																									// load map
		mySystem->setLevelChanging(false);																			// reset level changing flag
	}
}

void MyGame::render() {
	mySystem->render(gfx);																							// render world
}

int MyGame::rightAlignString(const std::string& string, int charWidth) {
	int width = 0, height = 0;																						// zero initialise width and height
	Dimension2i winSize = gfx->getCurrentWindowSize();																// get window size
	if (TTF_SizeText(font, string.c_str(), &width, &height) == 0 && width > 0) return winSize.w - width;			// IF TTF can get width, use it
	return winSize.w - static_cast<int>(string.length()) * charWidth;												// return estimated width
}

void MyGame::renderUI() {
	SDL_Color bg = SDL_COLOR_GRAY;																					// background colour
	bg.a = 128;																										// set alpha
	int bgHeight = DEFAULT_FONT_SIZE * 2;																			// background height
	Dimension2i winSize = gfx->getCurrentWindowSize();																// get window size
	// TOP OF SCREEN BACKGROUND
	gfx->setDrawColor(bg);																							// set colour
	gfx->fillRect(0, 0, winSize.w, bgHeight);																		// fill rect
	gfx->setDrawColor(SDL_COLOR_BLACK);																				// set colour
	gfx->drawRect(0, 0, winSize.w, bgHeight);																		// draw rect border
	// BOTTOM OF SCREEN BACKGROUND
	gfx->setDrawColor(bg);																							// set colour
	gfx->fillRect(0, winSize.h - bgHeight, winSize.w, bgHeight);													// fill rect
	gfx->setDrawColor(SDL_COLOR_BLACK);																				// set colour
	gfx->drawRect(0, winSize.h - bgHeight, winSize.w, bgHeight);													// draw rect border
	// UI TOP TEXT: SCORE (LEFT ALIGNED)
	gfx->setDrawColor(SDL_COLOR_WHITE);																				// set colour
	std::string scoreStr = "SCORE: " + std::to_string(mySystem->getScore());										// score string
	gfx->drawText(scoreStr, DEFAULT_FONT_SIZE, 8);																	// draw
	// UI TOP TEXT: ZOMBIES REMAINING (RIGHT ALIGNED)
	gfx->setDrawColor(SDL_COLOR_RED);																				// set colour
	std::string npcStr = "ZOMBIES REMAINING: " + std::to_string(mySystem->getNPCCount());							// NPC count string
	gfx->drawText(npcStr, rightAlignString(npcStr) - DEFAULT_FONT_SIZE, 8);											// draw right aligned
	// UI BOTTOM TEXT: HEALTH (LEFT ALIGNED)
	gfx->setDrawColor(SDL_COLOR_GREEN);																				// set colour
	std::string healthStr = "HEALTH: " + std::to_string(mySystem->getEntityHealth(playerEntityId));					// health string
	gfx->drawText(healthStr, DEFAULT_FONT_SIZE, winSize.h - bgHeight + 8);											// draw
	// UI BOTTOM TEXT: AMMO (RIGHT ALIGNED)
	gfx->setDrawColor(SDL_COLOR_ORANGE);																			// set colour
	std::string ammoStr = "AMMO: " + std::to_string(mySystem->getAmmo(playerEntityId));								// ammo string
	gfx->drawText(ammoStr, rightAlignString(ammoStr) - DEFAULT_FONT_SIZE, winSize.h - bgHeight + 8);				// draw right aligned
	if (gameWon) gfx->drawText("YOU WON", winSize.w / 2, winSize.h * 3 / 4);										// draw win message
}

void MyGame::loadResources() {
	// PLAYER SPRITES
	mySystem->loadSprite("player_idle_down", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 0, false);
	mySystem->loadSprite("player_idle_right", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 3, false);
	mySystem->loadSprite("player_idle_up", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 6, false);
	mySystem->loadSprite("player_walk_down", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 0, true);
	mySystem->loadSprite("player_walk_right", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 3, true);
	mySystem->loadSprite("player_walk_up", "res/player_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 6, true);
	mySystem->loadSprite("player_death_down", "res/player_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 0, false);
	mySystem->loadSprite("player_death_right", "res/player_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 3, false);
	mySystem->loadSprite("player_death_up", "res/player_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 6, false);
	// NPC SPRITES
	mySystem->loadSprite("zombie_idle_down", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 0, false);
	mySystem->loadSprite("zombie_idle_right", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 3, false);
	mySystem->loadSprite("zombie_idle_up", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 1, 6, false);
	mySystem->loadSprite("zombie_walk_down", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 0, true);
	mySystem->loadSprite("zombie_walk_right", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 3, true);
	mySystem->loadSprite("zombie_walk_up", "res/zombie_walk_sheet.png", TILE_SIZE, TILE_SIZE, 3, 6, true);
	mySystem->loadSprite("zombie_death_down", "res/zombie_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 0, false);
	mySystem->loadSprite("zombie_death_right", "res/zombie_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 3, false);
	mySystem->loadSprite("zombie_death_up", "res/zombie_death_sheet.png", TILE_SIZE, TILE_SIZE, 3, 6, false);
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
	// ITEM SPRITES
	mySystem->loadSprite("bullet", "res/bullet.png", TILE_SIZE, TILE_SIZE, 1, 0, false, 0.5f);
	mySystem->loadSprite("healthPickUp", "res/health.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("ammoPickup", "res/ammo.png", TILE_SIZE, TILE_SIZE, 1, 0);
	mySystem->loadSprite("endLevel", "res/endLevel.png", TILE_SIZE, TILE_SIZE, 1, 0);
	// LOAD SOUNDS
	mySystem->loadSound("shoot", "res/sfx/shoot.wav");
	mySystem->loadSound("zombie_hit_sound", "res/sfx/zombie_hit.wav");
	mySystem->loadSound("player_hit_sound", "res/sfx/player_death.wav");
	mySystem->loadSound("heal_sound", "res/sfx/heal_sound.wav");
	mySystem->loadSound("ammo_sound", "res/sfx/ammo.wav");
	mySystem->loadSound("endLevel_sound", "res/sfx/endLevel_sound.wav");
}

void MyGame::loadMap() {
	for (int row = 0; row < LEVEL_ROWS; ++row) {																// FOR EACH ROW
		for (int col = 0; col < LEVEL_COLS; ++col) {															// FOR EACH COLUMN
			int posX = col * TILE_SIZE;																			// position x
			int posY = row * TILE_SIZE;																			// position y
			int groundTileType = getRandom(1, 5);																// random ground tile type
			std::string groundAnim = "ground";																	// ground animation
			switch (groundTileType) {																			// Set ground animation based on random type
			case 1: groundAnim = "ground"; break;
			case 2: groundAnim = "ground2"; break;
			case 3: groundAnim = "ground3"; break;
			case 4: groundAnim = "ground4"; break;
			default: groundAnim = "ground"; break;
			}
			mySystem->addGroundTile(groundAnim, posX, posY);													// add ground tile
			int slotContent = levelmap.map[mySystem->getCurrentLevel()][row][col];								// get slot content
			switch (slotContent) {																				// SWITCH BASED ON SLOT CONTENT
			case 1: { uint32_t id = spawnBlock(float(posX), float(posY)); 	blockIds.push_back(id); break; }	// spawn block
			case 2: {
				if (playerEntityId >= 0)																		// IF PLAYER EXISTS
					mySystem->setEntityPosition(playerEntityId, Vector2f(float(posX), float(posY)));			// move existing player
				else playerEntityId = spawnPC(float(posX), float(posY));										// spawn player
				break;
			}
			case 3: { spawnNPC(float(posX), float(posY)); break; }												// spawn NPC
			case 4: { mySystem->addGroundTile("road", posX, posY);	 break; }									// road tile
			case 5: { mySystem->addGroundTile("roadH", posX, posY);	 break; }									// road horizontal tile
			case 6: { mySystem->addGroundTile("roadV", posX, posY); break; }									// road vertical tile
			case 10: { spawnEndLevelTrigger(float(posX), float(posY)); break; }									// spawn end level trigger
			default: break;
			}
		}
	}
	trySpawnItem(numAmmo, [this](float x, float y) { return spawnAmmoPickup(x, y); });							// try spawn ammo pickups
	trySpawnItem(numHealth, [this](float x, float y) { return spawnHealthPickup(x, y); });						// try spawn health pickups
}

void MyGame::trySpawnItem(int count, std::function<int(float, float)> func) {
	const int maxAttemptsPerItem = 50;																			// attempts per item
	for (int i = 0; i < count; ++i) {																			// FOR EACH ITEM TO SPAWN
		bool spawned = false;																					// spawned flag
		for (int attempt = 0; attempt < maxAttemptsPerItem && !spawned; ++attempt) {							// FOR EACH ATTEMPT
			int randCol = getRandom(TILE_SIZE, worldWidth - TILE_SIZE);											// random x
			int randRow = getRandom(TILE_SIZE, worldHeight - TILE_SIZE);										// random y
			SDL_Rect itemRect = { randCol, randRow, TILE_SIZE, TILE_SIZE };										// item rect
			bool collision = false;																				// collision flag
			for (int blockId : blockIds) {																		// FOR EACH BLOCK
				SDL_Rect blockRect = mySystem->getEntityColliderRect(blockId);									// get block rect
				if (SDL_HasIntersection(&blockRect, &itemRect)) { collision = true; break; }					// IF INTERSECTION, set collision flag and break
			}
			if (!collision) {																					// IF NO COLLISION
				int id = func(float(randCol), float(randRow));													// spawn item
				blockIds.push_back(id);																			// store pickup ID
				spawned = true;																					// set spawned flag
			}
		}
	}
}

uint32_t MyGame::spawnPC(float x, float y)
{
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentPCTag(entity);																			// add PC tag component 
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, OBJECT_LAYER);									// add transform component
	mySystem->addComponentVelocity(entity);																			// add velocity component
	mySystem->addComponentSpeed(entity, DEFAULT_PC_SPEED);															// add speed component
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	mySystem->addComponentHealth(entity, DEFAULT_MAX_HEALTH, DEFAULT_MAX_HEALTH);									// add health component
	mySystem->addComponentHealthBar(entity);																		// add health bar component
	mySystem->addComponentAmmo(entity, DEFAULT_MAX_AMMO / 2, DEFAULT_MAX_AMMO);										// add ammo component
	mySystem->addComponentDamage(entity, 25);																		// add damage component
	mySystem->addComponentInput(entity);																			// add input component
	mySystem->addComponentDying(entity);																			// add dying component
	mySystem->addComponentIdleAnimations(entity, "player_idle_down", "player_idle_right", "player_idle_up");		// add animation state component for idle
	mySystem->addComponentWalkAnimations(entity, "player_walk_down", "player_walk_right", "player_walk_up");		// add animation state component for walk
	mySystem->addComponentDeathAnimations(entity, "player_death_down", "player_death_right", "player_death_up");	// add animation state component for death
	mySystem->addComponentAudio(entity, "player_hit_sound", "shoot");												// add audio component
	mySystem->attachSprite(entity, "player_idle_down");																// add default sprite
	mySystem->initProjectilePool(entity, DEFAULT_PROJECTILES_PER_OWNER);											// add projectile pool
	return entity;																									// return entity
}

uint32_t MyGame::spawnNPC(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentNPCTag(entity);																			// add NPC tag component
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, OBJECT_LAYER, true);								// add transform component
	mySystem->addComponentVelocity(entity);																			// add velocity component
	mySystem->addComponentSpeed(entity, DEFAULT_UNIT_SPEED);														// add speed component
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	mySystem->addComponentHealth(entity, DEFAULT_MAX_HEALTH / 2, DEFAULT_MAX_HEALTH);								// add health component
	mySystem->addComponentHealthBar(entity);																		// add health bar component
	mySystem->addComponentDamage(entity, DEFAULT_UNIT_DAMAGE);														// add damage component
	mySystem->addComponentDying(entity);																			// add dying component
	mySystem->addComponentIdleAnimations(entity, "zombie_idle_down", "zombie_idle_right", "zombie_idle_up");		// add animation state component for idle
	mySystem->addComponentWalkAnimations(entity, "zombie_walk_down", "zombie_walk_right", "zombie_walk_up");		// add animation state component for walk
	mySystem->addComponentDeathAnimations(entity, "zombie_death_down", "zombie_death_right", "zombie_death_up");	// add animation state component for death
	mySystem->addComponentAudio(entity, "zombie_hit_sound", "");													// add audio component
	mySystem->attachSprite(entity, "zombie_idle_down");																// add default sprite
	mySystem->addComponentScoreValue(entity, 10);																	// add score value component
	return entity;																									// return entity
}

uint32_t MyGame::spawnBlock(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->attachSprite(entity, "block");																		// add block sprite
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, OBJECT_LAYER);									// add transform component
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	return entity;																									// return entity
}

uint32_t MyGame::spawnAmmoPickup(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentAmmoPickupTag(entity);																	// add ammo pickup tag component
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, OBJECT_LAYER);									// add transform component
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	mySystem->attachSprite(entity, "ammoPickup");																	// add ammo pickup sprite
	mySystem->addComponentAudio(entity, "", "ammo_sound");															// add audio component
	return entity;																									// return entity
}

uint32_t MyGame::spawnHealthPickup(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentHealthPickupTag(entity);																	// add health pickup tag component
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, OBJECT_LAYER);									// add transform component
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	mySystem->addComponentDamage(entity, -25);																		// add damage component (negative for healing)
	mySystem->attachSprite(entity, "healthPickUp");																	// add health pickup sprite
	mySystem->addComponentAudio(entity, "", "heal_sound");															// add audio component
	return entity;																									// return entity
}

uint32_t MyGame::spawnEndLevelTrigger(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentEndLevelTag(entity);																		// add end level tag component
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, GROUND_LAYER);									// add transform component 
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// add collider component
	mySystem->attachSprite(entity, "endLevel");																		// add end level sprite
	mySystem->addComponentAudio(entity, "", "endLevel_sound");														// add audio component
	return entity;																									// return entity
}