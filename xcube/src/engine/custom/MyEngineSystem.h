#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../ResourceManager.h"									// For resource loading

#include <unordered_map>										// For component storage
#include <utility>												// for std::pair

static constexpr int DEFAULT_ENTITY_ID = { -1 };				// Default entity ID
static constexpr Uint32 DEFAULT_ENTITY_WIDTH = { 16 };			// Default width
static constexpr Uint32 DEFAULT_ENTITY_HEIGHT = { 16 };			// Default height
static constexpr float DEFAULT_ENTITY_SCALE = { 1.0f };			// Default scale
static constexpr float FIXED_DT = { 1.0f / 60.0f };				// fixed timestep	
static constexpr int DEFAULT_MAX_HEALTH = { 100 };				// Default max health
static constexpr Uint32 DEFAULT_AMMO = { 10 };					// initial ammo count
static constexpr Uint32 DEFAULT_MAX_AMMO = { 50 };				// maximum ammo count
static constexpr float DEFAULT_UNIT_SPEED = { 100 };			// Default speed
static constexpr float DEFAULT_PC_SPEED = { 200 };				// Default speed
static constexpr float deltaTime = { 1.0f / 60.0f };			// fixed delta time
static constexpr Uint32 STAT_CHANGE_COOLDOWN = { 250 };			// stat change cooldown in milliseconds
static constexpr Uint32 BAR_HEIGHT = { 4 };						// height of health/ammo bars
static constexpr Uint32 DEFAULT_UNIT_DAMAGE = { 10 };			// Default unit damage
static constexpr Uint32 DEFAULT_NPC_CHASE_RANGE = { 65536 };	// Default NPC chase radius (256^2)

class MyEngineSystem {
	friend class XCube2Engine;									// Friend class declaration
private:
	MyEngineSystem();											// Constructor
	using Entity = std::uint32_t;								// Entity ID type
	template<typename T>
	using ComponentMap = std::unordered_map<Entity, T>;			// Component storage map
	struct PCTag {};											// PC Tag
	struct NPCTag {};											// NPC Character Tag
	struct Transform {											// A structure to hold transform data
		Vector2f position = {};									// Position
		int rotation = {};										// Rotation in degrees
		bool initialFlipH = false;								// Initial horizontal flip (original image flipped)
		bool flipH = false;										// Horizontal flip (for rendering)
		Vector2f newPosition = {};								// New position after movement
	};
	struct Velocity {											// A structure to hold velocity data
		float x = {};											// Velocity X
		float y = {};											// Velocity Y
	};
	struct Sprite {												// A structure to hold sprite data
		SDL_Texture* texture = nullptr;							// Texture containing all frames
		int frameW = {};										// Width of a single frame
		int frameH = {};										// Height of a single frame
		int frameCount = 1;										// Total number of frames
		int textureWidth = {};									// Full texture width
		int textureHeight = {};  								// Full texture height
		int startFrame = {};									// Starting frame index
	};
	struct Animation {											// A structure to hold animation data
		std::string name;										// Animation name (eg "player_walk_down")
		bool loop = true;										// Does the animation loop
		int frameCount = 1;										// Total number of frames (cached from Sprite)
		float frameDuration = 0.1f;								// seconds per frame (cached from Sprite)
		float animTimer = {};									// Timer for animation frame switching
		int currentFrame = {};									// Current frame index (0..frameCount-1)
	};
	struct AnimationState {										// A structure to hold animation state data
		std::string idle_up = {};								// Idle up animation name
		std::string idle_down = {};								// Idle down animation name
		std::string idle_right = {};							// Idle right animation name
		std::string walk_up = {};								// Walk up animation name
		std::string walk_down = {};								// Walk down animation name
		std::string walk_right = {};							// Walk right animation name
		std::string death_up = {};								// Death up animation name
		std::string death_down = {};							// Death down animation name
		std::string death_right = {};							// Death right animation name
		std::string previousAnimation = {};						// Previous animation name
	};
	struct Health {												// A structure to hold health data
		int currentHealth = DEFAULT_MAX_HEALTH;					// Current health
		int maxHealth = DEFAULT_MAX_HEALTH;						// Maximum health
	};
	struct HealthBar {											// A structure to hold health bar data
		SDL_Rect backgroundRect = {};							// Background rectangle
		SDL_Rect healthRect = {};								// Health rectangle
	};
	struct Collider {											// A structure to hold collider data
		int width = DEFAULT_ENTITY_WIDTH;						// Collider width
		int height = DEFAULT_ENTITY_HEIGHT;						// Collider height
	};
	struct Damage {												// A structure to hold damage data
		int amount = DEFAULT_UNIT_DAMAGE;						// Damage amount
	};
	struct Speed {												// A structure to hold speed data
		float value = DEFAULT_UNIT_SPEED;						// Speed value
	};
	struct Ammo {												// A structure to hold ammo data
		int currentAmmo = DEFAULT_AMMO;							// Current ammo
		int maxAmmo = DEFAULT_MAX_AMMO;							// Maximum ammo
	};
	struct Input {												// A structure to hold input data
		float x = {};											// Input X
		float y = {};											// Input Y
	};
	ComponentMap<Input> inputs;									// Input component storage
	struct Component											// Component storage struct
	{
		ComponentMap<Transform> transforms;						// Transform Component storage
		ComponentMap<Velocity> velocities;						// Velocity Component storage
		ComponentMap<Sprite> sprites;							// Sprite Component storage
		ComponentMap<Animation> animations;						// Animation Component storage
		ComponentMap<PCTag> players;							// Player Tag Component storage
		ComponentMap<NPCTag> npcs;								// NPC Tag Component storage
		ComponentMap<Health> healths;							// Health Component storage
		ComponentMap<Collider> colliders;						// Collider Component storage
		ComponentMap<Damage> damages;							// Damage Component storage
		ComponentMap<Speed> speeds;								// Speed Component storage
		ComponentMap<Ammo> ammos;								// Ammo Component storage
		ComponentMap<HealthBar> healthBars;						// HealthBar Component storage
		ComponentMap<Input> inputs;								// Input Component storage
		ComponentMap<AnimationState> animationStates;			// AnimationState Component storage
	};
	Component component;										// Exposed Registry instance
	std::vector<Entity> entities;   							// Created entities
	std::map<std::string, Mix_Chunk*> sounds;					// Loaded sounds
	std::map<std::string, Sprite> sprites;						// Loaded sprite data keyed by name
	int nextEntityId;   										// For generating unique entity IDs
	struct Tile {
		int x = {};												// Tile X
		int y = {};												// Tile Y
		std::string spriteName;									// Sprite name
	};
	std::vector<Tile> groundTiles;								// Ground tiles


public:
	~MyEngineSystem();																	// Destructor
	Entity createEntity() { static Entity next = 1; return next++; };					// Create a new entity ID
	void loadSprite(const std::string& name, const std::string& filename, int frameW, int frameH, int frames, int startFrame = 0, SDL_Color transparent = { 255,255,255,255 });	// Load sprite from file
	void attachSprite(Entity entity, const std::string& spriteName);					// Attach sprite to entity
	void render(std::shared_ptr<GraphicsEngine> gfx);									// Render all entities
	void setEntityInput(Entity entity, float x, float y);								// Set entity input
	void movementSystem(Component& com, float deltaTime = deltaTime);					// Movement system
	void update(float deltaTime = deltaTime, int playerEntityId = 1);					// Update all systems
	void animationSystem(Component& com, float deltaTime = deltaTime);					// Animation system
	void updateAnimationStates(Component& com, float deltaTime = deltaTime);			// Update animation states
	Entity spawnPC(float x, float y, float scale = DEFAULT_ENTITY_SCALE);				// Spawn player character
	Entity spawnNPC(float x, float y, float scale = DEFAULT_ENTITY_SCALE);				// Spawn non-player character
	Entity spawnBlock(float x, float y, float scale = DEFAULT_ENTITY_SCALE);			// Spawn block entity
	void renderHealthBar(std::shared_ptr<GraphicsEngine> gfx, Entity entity, int posX, int posY, int width, int height);	// Render health bar
	void renderTiles(std::shared_ptr<GraphicsEngine>);									// Render ground tiles
	void addGroundTile(const std::string& spriteName, int x, int y);					// Add ground tile
	void collisionSystem(Component& com, float deltaTime = deltaTime);					// Collision system
	void aiSystem(Component& com, Entity playerEntity, float deltaTime = deltaTime);	// AI system
	int getEntityHealth(Entity entity);													// Get entity health
	void loadSound(const std::string& name, const std::string& filename);				// Load sound
};

#endif 

