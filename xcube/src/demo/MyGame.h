#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"											// for AbstractGame
#include "../engine/Level.h"												// for Level
#include <functional>														// for std::function

static constexpr Uint32 WINDOW_WIDTH = 800;									// window dimensions
static constexpr Uint32 WINDOW_HEIGHT = 600;								// window dimensions
static constexpr Uint32 DEFAULT_FONT_SIZE = { 24 };							// default font size
static const SDL_Color SDL_COLOR_DARK_GRAY = { 70, 70, 70 };				// dark gray color

class MyGame : public AbstractGame {
private:
	int numAmmo, numHealth, lives;											// game stats
	bool gameWon;															// win state
	std::vector<uint32_t> npcIds;												// NPC entity IDs
	std::vector<uint32_t> blockIds;												// Block entity IDs
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
	void trySpawnItem(int count, std::function<int(float, float)> func);	// try spawn pickups
	uint32_t spawnPC(float x, float y);										// spawn player character
	uint32_t spawnNPC(float x, float y);									// spawn NPC
	uint32_t spawnBlock(float x, float y);									// spawn block
	uint32_t spawnAmmoPickup(float x, float y);								// spawn ammo pickup
	uint32_t spawnHealthPickup(float x, float y);							// spawn health pickup
public:
	MyGame();																// constructor
	~MyGame() = default;													// destructor (calls base)
};

#endif