#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"									// for AbstractGame
#include "../engine/Level.h"										// for Level

constexpr int WINDOW_WIDTH = 800;									// window dimensions
constexpr int WINDOW_HEIGHT = 600;									// window dimensions
constexpr int TILE_SIZE = 16;										// tile size in pixels	


class MyGame : public AbstractGame {
private:
	int score, numAmmo, numHealth, lives;							// game stats
	bool gameWon;													// win state
	std::vector<int> npcIds;										// NPC entity IDs
	std::vector<int> blockIds;										// Block entity IDs
	Level levelmap = {};											// level map
	int worldWidth = LEVEL_COLS * TILE_SIZE;						// world dimensions
	int worldHeight = LEVEL_ROWS * TILE_SIZE;						// world dimensions
	void handleKeyEvents();											// handle key events
	void update();													// update
	void render();													// render
	void renderUI();												// render UI
	void loadMap();													// load level map
	void loadResources();											// load resources
	int playerEntityId = -1;										// -1 = not spawned
public:
	MyGame();														// constructor
	~MyGame() = default;											// destructor (calls base)
};

#endif