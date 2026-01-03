#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), numAmmo(5), numHealth(3), gameWon(false) {
	srand((unsigned int)time(nullptr));																// Seed random number generator
	font = ResourceManager::loadFont("res/fonts/arial.ttf", DEFAULT_FONT_SIZE);						// Load font
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

void MyGame::onLeftMouseButton() {
	if (!mySystem) return;																			// safety check
	if (playerEntityId < 0) return;																	// player not spawned
	if (mousePressed) return;																		// already pressed
	mousePressed = true;																			// set pressed
	Vector2i mouseScreenPos = Vector2i(eventSystem->getMousePos().x, eventSystem->getMousePos().y);	// get mouse screen pos
	Vector2f cameraPos = mySystem->getCameraPosition();												// get camera pos
	Vector2f mouseWorldPos = Vector2f(float(mouseScreenPos.x) + cameraPos.x, float(mouseScreenPos.y) + cameraPos.y);	// convert to world pos
	Vector2f playerPos = mySystem->getEntityPosition(playerEntityId);								// get player pos
	mySystem->fireProjectile(playerEntityId, playerPos, mouseWorldPos);								// player shoot
}

void MyGame::update() {
	if (eventSystem && !eventSystem->isPressed(Mouse::BTN_LEFT)) mousePressed = false;				// reset mouse pressed state
	mySystem->update(FIXED_DT, playerEntityId);														// movement system
	if (mySystem->getNPCCount() == 0) gameWon = true;												// check win condition
}

void MyGame::render() {
	mySystem->render(gfx);																			// render world
}

int MyGame::rightAlignString(const std::string& string, int charWidth) {
	int width = 0, height = 0;																		// zero initialise width and height
	if (TTF_SizeText(font, string.c_str(), &width, &height) == 0 && width > 0)						// IF STRING MEASUREMENT FROM FONT SUCCESSFUL
		return WINDOW_WIDTH - width;																// return calculated width
	return WINDOW_WIDTH - static_cast<int>(string.length()) * charWidth;							// return estimated width
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_GRAY);																// set color
	gfx->fillRect(0, 0, WINDOW_WIDTH, DEFAULT_FONT_SIZE * 3);										// fill rect
	gfx->setDrawColor(SDL_COLOR_DARK_GRAY);															// set color
	gfx->drawRect(0, 0, WINDOW_WIDTH, DEFAULT_FONT_SIZE * 3);										// draw rect border
	gfx->setDrawColor(SDL_COLOR_AQUA);																// set color
	std::string scoreStr = "SCORE: " + std::to_string(mySystem->getScore());						// score string
	gfx->drawText(scoreStr, rightAlignString(scoreStr) - DEFAULT_FONT_SIZE, 8);						// draw score right aligned
	gfx->setDrawColor(SDL_COLOR_GREEN);																// set color
	std::string healthStr = "HEALTH: " + std::to_string(mySystem->getEntityHealth(playerEntityId));	// health string
	gfx->drawText(healthStr, DEFAULT_FONT_SIZE, 8);													// draw
	gfx->setDrawColor(SDL_COLOR_RED);																// set color
	std::string ammoStr = "AMMO: " + std::to_string(mySystem->getAmmo(playerEntityId));				// ammo string
	gfx->drawText(ammoStr, DEFAULT_FONT_SIZE, 32);													// draw
	std::string npcStr = "ZOMBIES LEFT: " + std::to_string(mySystem->getNPCCount());				// NPC count string
	gfx->drawText(npcStr, rightAlignString(npcStr) - DEFAULT_FONT_SIZE, 32);						// draw right aligned
	if (gameWon) gfx->drawText("YOU WON", WINDOW_WIDTH / 2, WINDOW_HEIGHT * 3 / 4);					// draw win message
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
				uint32_t id = spawnBlock(float(posX), float(posY));									// spawn block
				blockIds.push_back(id);																// store block ID
				break;
			}
			case 2: {
				playerEntityId = spawnPC(float(posX), float(posY));									// spawn player
				break;
			}
			case 3: {
				uint32_t id = spawnNPC(float(posX), float(posY));									// spawn NPC
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
	trySpawnItem(numAmmo, [this](float x, float y) { return spawnAmmoPickup(x, y); });				// try spawn ammo pickups
	trySpawnItem(numHealth, [this](float x, float y) { return spawnHealthPickup(x, y); });			// try spawn health pickups
}

uint32_t MyGame::spawnPC(float x, float y)
{
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentPCTag(entity);																			// set PC tag
	mySystem->addComponentTransform(entity, Vector2f(x, y));														// set transform
	mySystem->addComponentVelocity(entity);																			// set velocity
	mySystem->addComponentSpeed(entity, DEFAULT_PC_SPEED);															// set speed
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// set collider
	mySystem->addComponentHealth(entity, DEFAULT_MAX_HEALTH, DEFAULT_MAX_HEALTH);									// set health
	mySystem->addComponentHealthBar(entity);																		// set health bar
	mySystem->addComponentAmmo(entity, DEFAULT_MAX_AMMO / 2, DEFAULT_MAX_AMMO);										// set ammo
	mySystem->addComponentDamage(entity, DEFAULT_UNIT_DAMAGE);														// set damage
	mySystem->addComponentInput(entity);																			// set input
	mySystem->addComponentDying(entity);																			// set dying state
	mySystem->addComponentIdleAnimations(entity, "player_idle_down", "player_idle_right", "player_idle_up");		// set idle animations
	mySystem->addComponentWalkAnimations(entity, "player_walk_down", "player_walk_right", "player_walk_up");		// set walk animations
	mySystem->addComponentDeathAnimations(entity, "player_death_down", "player_death_right", "player_death_up");	// set death animations
	mySystem->addComponentAudio(entity, "player_hit_sound", "shoot");												// set audio component
	mySystem->attachSprite(entity, "player_idle_down");																// attach default sprite
	mySystem->initProjectilePool(entity, DEFAULT_PROJECTILES_PER_OWNER);											// initialise projectile pool
	return entity;																									// return entity
}

uint32_t MyGame::spawnNPC(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentNPCTag(entity);																			// set NPC tag
	mySystem->addComponentTransform(entity, Vector2f(x, y), 1, 0, true);											// set transform
	mySystem->addComponentVelocity(entity);																			// set velocity
	mySystem->addComponentSpeed(entity, DEFAULT_UNIT_SPEED);														// set speed
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// set collider
	mySystem->addComponentHealth(entity, DEFAULT_MAX_HEALTH / 2, DEFAULT_MAX_HEALTH);								// set health
	mySystem->addComponentHealthBar(entity);																		// set health bar
	mySystem->addComponentDamage(entity, DEFAULT_UNIT_DAMAGE);														// set damage
	mySystem->addComponentDying(entity);																			// set dying state
	mySystem->addComponentIdleAnimations(entity, "zombie_idle_down", "zombie_idle_right", "zombie_idle_up");		// set idle animations
	mySystem->addComponentWalkAnimations(entity, "zombie_walk_down", "zombie_walk_right", "zombie_walk_up");		// set walk animations
	mySystem->addComponentDeathAnimations(entity, "zombie_death_down", "zombie_death_right", "zombie_death_up");	// set death animations
	mySystem->addComponentAudio(entity, "zombie_hit_sound", "");													// set audio component
	mySystem->attachSprite(entity, "zombie_idle_down");																// attach default sprite
	mySystem->addComponentScoreValue(entity, 10);																	// set score value
	return entity;																									// return entity
}

uint32_t MyGame::spawnBlock(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->attachSprite(entity, "block");																		// attach block sprite
	mySystem->addComponentTransform(entity, Vector2f(x, y));														// set
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// set collider
	return entity;																									// return entity
}

uint32_t MyGame::spawnAmmoPickup(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentAmmoPickupTag(entity);																	// add ammo pickup tag
	mySystem->addComponentTransform(entity, Vector2f(x, y));														// set transform
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// set collider
	mySystem->attachSprite(entity, "ammoPickup");																	// attach ammo pickup sprite
	mySystem->addComponentAudio(entity, "", "ammo_sound");															// store audio component
	return entity;																									// return entity
}

uint32_t MyGame::spawnHealthPickup(float x, float y) {
	uint32_t entity = mySystem->createEntity();																		// create entity
	mySystem->addComponentHealthPickupTag(entity);																	// add health pickup tag
	mySystem->addComponentTransform(entity, Vector2f(x, y));														// set transform
	mySystem->addComponentCollider(entity, x, y, TILE_SIZE, TILE_SIZE);												// set collider
	mySystem->addComponentDamage(entity, -25);																		// set negative damage for healing
	mySystem->attachSprite(entity, "healthPickUp");																	// attach health pickup sprite
	mySystem->addComponentAudio(entity, "", "heal_sound");															// store audio component
	return entity;																									// return entity
}

void MyGame::trySpawnItem(int count, std::function<int(float, float)> func) {
	int iterations = 5;																								// reset iterations
	for (int i = 0; i < count; ++i)																					// FOR EACH HEALTH PICKUP TO SPAWN
		while (iterations-- > 0) {																					// WHILE ATTEMPTS REMAIN
			int randCol = getRandom(TILE_SIZE * 1.5, worldWidth - (TILE_SIZE * 1.5));								// random column
			int randRow = getRandom(TILE_SIZE * 1.5, worldHeight - (TILE_SIZE * 1.5));								// random row
			for (int blockId : blockIds) {																			// FOR EACH BLOCK
				SDL_Rect blockRect = mySystem->getEntityColliderRect(blockId);										// block rect
				SDL_Rect itemRect = { randCol, randRow, TILE_SIZE, TILE_SIZE };										// item rect
				if (SDL_HasIntersection(&blockRect, &itemRect))  break;												// IF INTERSECTS BLOCK, break
				else {																								// ELSE
					int id = func(float(randCol), float(randRow));													// spawn item
					blockIds.push_back(id);																			// store pickup ID
					break;																							// break
				}
			}
		}
}