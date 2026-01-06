#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"											// for AbstractGame
#include "../engine/Level.h"												// for Level
#include <functional>														// for std::function

class MyGame : public AbstractGame {
private:
	int numAmmo, numHealth, lives;											// game stats
	bool gameWon;															// win state
	std::vector<uint32_t> blockIds, otherEntities;							// Block entity IDs
	Level levelmap = {};													// level map
	int worldWidth = LEVEL_COLS * TILE_SIZE;								// world dimensions
	int worldHeight = LEVEL_ROWS * TILE_SIZE;								// world dimensions
	int playerEntityId = { -1 };											// -1 = not spawned
	bool mousePressed = false;												// mouse pressed state
	TTF_Font* font = nullptr;												// font
	void handleKeyEvents();													// handle key events
	void onLeftMouseButton();												// handle mouse events
	void update();															// update
	void render();															// render
	void renderUI();														// render UI
	void loadMap();															// load level map
	void loadResources();													// load resources
	int rightAlignString(const std::string& string, int charWidth = 24);	// get UI string width
	// SPAWN METHODS
	void trySpawnItem(Uint32 count, std::function<Uint32(float, float)> func);	// try spawn pickups
	uint32_t spawnPC(float x, float y);										// spawn player character
	uint32_t spawnNPC(float x, float y);									// spawn NPC
	uint32_t spawnBlock(float x, float y);									// spawn block
	uint32_t spawnAmmoPickup(float x, float y);								// spawn ammo pickup
	uint32_t spawnHealthPickup(float x, float y);							// spawn health pickup
	uint32_t spawnEndLevelTrigger(float x, float y);						// spawn end level trigger
public:
	MyGame();																// constructor
	~MyGame();																// destructor
};

#endif