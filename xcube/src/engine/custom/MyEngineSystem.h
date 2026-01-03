#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__
#include "../ResourceManager.h"																	// For resource loading
#include <unordered_map>																		// For component storage
#include <utility>																				// for std::pair

static constexpr int DEFAULT_ENTITY_ID = { -1 };												// Default entity ID
static constexpr Uint32 TILE_SIZE = { 16 };														// tile size in pixels	
static constexpr float DEFAULT_ENTITY_SCALE = { 1.0f };											// Default scale
static constexpr float FIXED_DT = { 1.0f / 60.0f };												// fixed timestep	
static constexpr int DEFAULT_MAX_HEALTH = { 100 };												// Default max health
static constexpr Uint32 DEFAULT_AMMO = { 10 };													// initial ammo count
static constexpr Uint32 DEFAULT_MAX_AMMO = { 50 };												// maximum ammo count
static constexpr float DEFAULT_UNIT_SPEED = { 100 };											// Default speed
static constexpr float DEFAULT_PC_SPEED = { 200 };												// Default speed
static constexpr float deltaTime = { 1.0f / 60.0f };											// fixed delta time
static constexpr Uint32 STAT_CHANGE_COOLDOWN = { 250 };											// stat change cooldown in milliseconds
static constexpr Uint32 BAR_HEIGHT = { 4 };														// height of health/ammo bars
static constexpr Uint32 DEFAULT_UNIT_DAMAGE = { 10 };											// Default unit damage
static constexpr Uint32 DEFAULT_NPC_CHASE_RANGE = { 256 * 256 };								// Default NPC chase radius
static constexpr size_t DEFAULT_PROJECTILES_PER_OWNER = 50;										// default bullets per owner
static constexpr Uint32 DEFAULT_NPC_SCORE_VALUE = { 10 };										// default score per NPC
static constexpr Uint32 DEFAULT_SFX_VOLUME = { 10 };											// default sfx volume
static constexpr float OFFSCREEN_X = { -1000.0f };												// offscreen x position
static constexpr float OFFSCREEN_Y = { -1000.0f };												// offscreen y position
static constexpr float DEFAULT_PROJECTILE_SPEED = { 800.0f };									// default projectile speed

class MyEngineSystem {
	friend class XCube2Engine;																	// Friend class declaration
private:
	MyEngineSystem();																			// Constructor
	using Entity = std::uint32_t;																// Entity ID type
	template<typename T>																		// Template for component map
	using ComponentMap = std::unordered_map<Entity, T>;											// Component storage map
	struct PCTag {};																			// PC Tag
	struct NPCTag {};																			// NPC Character Tag
	struct AmmoPickupTag {};																	// Ammo Pickup Tag
	struct HealthPickupTag {};																	// Health Pickup Tag
	struct ProjectileTag {																		// projectile component
		Entity owner = { 0 };																	// owner entity
	};
	struct Transform {																			// A structure to hold transform data
		Vector2f startPosition = {};															// Starting position
		Vector2f position = {};																	// Position
		float scale = DEFAULT_ENTITY_SCALE;														// Scale factor
		int rotation = {};																		// Rotation in degrees
		bool initialFlipH = false;																// Initial horizontal flip (original image flipped)
		bool flipH = false;																		// Horizontal flip (for rendering)
		Vector2f newPosition = {};																// New position after movement
		bool active = true; 																	// Is the entity active
	};
	struct Velocity {																			// A structure to hold velocity data
		float x = {};																			// Velocity X
		float y = {};																			// Velocity Y
	};
	struct Sprite {																				// A structure to hold sprite data
		SDL_Texture* texture = nullptr;															// Texture containing all frames
		int frameW = {};																		// Width of a single frame
		int frameH = {};																		// Height of a single frame
		int frameCount = 1;																		// Total number of frames
		int textureWidth = {};																	// Full texture width
		int textureHeight = {};  																// Full texture height
		int startFrame = {};																	// Starting frame index
		bool loop = true;																		// Does the sprite loop
		float scale = DEFAULT_ENTITY_SCALE;														// Scale factor
	};
	struct Animation {																			// A structure to hold animation data
		std::string name;																		// Animation name (eg "player_walk_down")
		bool loop = true;																		// Does the animation loop
		int frameCount = 1;																		// Total number of frames (cached from Sprite)
		float frameDuration = 0.1f;																// seconds per frame (cached from Sprite)
		float animTimer = {};																	// Timer for animation frame switching
		int currentFrame = {};																	// Current frame index (0..frameCount-1)
	};
	struct AnimationState {																		// A structure to hold animation state data
		std::string idle_up = {};																// Idle up animation name
		std::string idle_down = {};																// Idle down animation name
		std::string idle_right = {};															// Idle right animation name
		std::string walk_up = {};																// Walk up animation name
		std::string walk_down = {};																// Walk down animation name
		std::string walk_right = {};															// Walk right animation name
		std::string death_up = {};																// Death up animation name
		std::string death_down = {};															// Death down animation name
		std::string death_right = {};															// Death right animation name
		std::string previousAnimation = {};														// Previous animation name
	};
	struct Audio {																				// A structure to hold audio data
		std::string damageSound = {};															// Sound to play on damage
		std::string attackingSound = {};														// Sound to play on attacking
	};
	struct Health {																				// A structure to hold health data
		int currentHealth = DEFAULT_MAX_HEALTH;													// Current health
		int maxHealth = DEFAULT_MAX_HEALTH;														// Maximum health
		Uint32 lastHealthChangeTime = STAT_CHANGE_COOLDOWN;										// Last time health was changed
	};
	struct HealthBar {																			// A structure to hold health bar data
		SDL_Rect backgroundRect = {};															// Background rectangle
		SDL_Rect healthRect = {};																// Health rectangle
	};
	struct Collider {																			// A structure to hold collider data
		SDL_Rect rect = {};																		// Collider rectangle
	};
	struct Damage {																				// A structure to hold damage data
		int amount = DEFAULT_UNIT_DAMAGE;														// Damage amount
		Uint32 lastDamageDealtTime = STAT_CHANGE_COOLDOWN;										// Last time damage was dealt
	};
	struct Speed {																				// A structure to hold speed data
		float value = DEFAULT_UNIT_SPEED;														// Speed value
	};
	struct Ammo {																				// A structure to hold ammo data
		int currentAmmo = DEFAULT_AMMO;															// Current ammo
		int maxAmmo = DEFAULT_MAX_AMMO;															// Maximum ammo
		Uint32 lastFireTime = STAT_CHANGE_COOLDOWN;												// Last time fired
	};
	struct Input {																				// A structure to hold input data
		float x = {};																			// Input X
		float y = {};																			// Input Y
	};

	struct ScoreValue {																			// Score value component
		int amount = {};																		// score amount
	};
	ComponentMap<Input> inputs;																	// Input component storage
	struct Component																			// Component storage struct
	{
		ComponentMap<Transform> transforms;														// Transform Component storage
		ComponentMap<Velocity> velocities;														// Velocity Component storage
		ComponentMap<Sprite> sprites;															// Sprite Component storage
		ComponentMap<Animation> animations;														// Animation Component storage
		ComponentMap<PCTag> players;															// Player Tag Component storage
		ComponentMap<NPCTag> npcs;																// NPC Tag Component storage
		ComponentMap<AmmoPickupTag> ammoPickups;												// Ammo Pickup Tag Component storage
		ComponentMap<HealthPickupTag> healthPickups;											// Health Pickup Tag
		ComponentMap<ProjectileTag> projectiles;													// Projectile Component storage
		ComponentMap<Health> healths;															// Health Component storage
		ComponentMap<Collider> colliders;														// Collider Component storage
		ComponentMap<Damage> damages;															// Damage Component storage
		ComponentMap<Speed> speeds;																// Speed Component storage
		ComponentMap<Ammo> ammos;																// Ammo Component storage
		ComponentMap<HealthBar> healthBars;														// HealthBar Component storage
		ComponentMap<Input> inputs;																// Input Component storage
		ComponentMap<AnimationState> animationStates;											// AnimationState Component storage
		ComponentMap<bool>dying;																// Dying state Component storage
		ComponentMap<Audio> audios;																// Audio Component storage
		ComponentMap<ScoreValue> scores;														// Score Component storage
	};
	Component component;																		// Exposed Registry instance
	std::vector<Entity> entities;   															// Created entities
	std::map<std::string, Sprite> sprites;														// Loaded sprite data keyed by name
	std::map<std::string, Mix_Chunk*> sounds;													// Loaded sounds
	std::unordered_map<Entity, std::vector<Entity>> projectilePools;							// owner pool of projectile entity IDs
	int nextEntityId;   																		// For generating unique entity IDs
	struct Tile {
		int x = {};																				// Tile X
		int y = {};																				// Tile Y
		std::string spriteName;																	// Sprite name
	};
	std::vector<Tile> groundTiles;																// Ground tiles
	Uint32 now = {};																			// Current time in milliseconds
	Uint32 score = {};																			// Player score
	std::vector<Entity> entitiesToDestroy;														// entities queued for destruction
	Vector2f cameraPosition{ 0.0f, 0.0f };														// camera world position
	float cameraSmoothing = 6.0f;																// camera smoothing factor
	// PRIVATE METHODS
	void flushDestroyedEntities();																// actually remove enqueued entities
	void destroyEntity(Entity entity);															// Destroy entity
	void movementSystem(Component& com, float deltaTime = deltaTime);							// Movement system
	void animationSystem(Component& com, float deltaTime = deltaTime);							// Animation system
	void updateAnimationStates(Component& com, float deltaTime = deltaTime);					// Update animation states
	void renderHealthBar(std::shared_ptr<GraphicsEngine> gfx, Entity entity, int posX, int posY, int width, int height);	// Render health bar
	void renderTiles(std::shared_ptr<GraphicsEngine>);											// Render ground tiles
	void collisionSystem(Component& com, float deltaTime = deltaTime);							// Collision system
	void aiSystem(Component& com, Entity playerEntity, float deltaTime = deltaTime);			// AI system
	void changeEntityHealth(Entity entity, int amount);											// Change entity health
	void processCollisionEntities(Component& com, Entity primary, Entity other, Uint32 now);	// Process collision between two entities
	void playAudio(const std::string& name, int volume = -1, int loops = 0, int channel = -1);	// Play audio
	void handleDeath(Entity entity, Health& health);											// Respawn entity
	void deactivateProjectile(Entity proj);														// deactivate projectile
	void updateCamera(std::shared_ptr<GraphicsEngine> gfx, float deltaTime = deltaTime);		// update camera position
	void increaseAmmo(Entity attacker, Entity victim);											// increase ammo for owner
	void processPendingDeaths();																// Check dying entities and finalize when anim done
	void finaliseDeath(Entity entity);															// Perform the actual death completion work
	bool isProjectileOwner(Entity entity, Entity other);										// check if projectile owner
	void setEntityColliderRect(Entity entity, float posX, float posY, int width, int height);	// set entity collider rectangle
	template<typename T>																		// Template for getting valid component
	bool isValidComponent(Entity entity, ComponentMap<T>& comp);								// check if entity has valid component
public:
	~MyEngineSystem();																			// Destructor
	Entity createEntity() { static Entity next = 1; return next++; };							// Create a new entity ID
	void loadSprite(const std::string& name, const std::string& filename, int frameW, int frameH, int frames, int startFrame = 0, bool loop = false, float scale = 1, SDL_Color transparent = { 255,255,255,255 });	// Load sprite from file
	void loadSound(const std::string& name, const std::string& filename);						// Load sound
	void render(std::shared_ptr<GraphicsEngine> gfx);											// Render all entities
	void update(float deltaTime = deltaTime, int playerEntityId = 1);							// Update all systems
	void addGroundTile(const std::string& spriteName, int x, int y);							// Add ground tile
	void fireProjectile(Entity owner, const Vector2f& startPos, const Vector2f& targetPos);		// fire a projectile from owner
	void setEntityInput(Entity entity, float x, float y);										// Set entity input
	void initProjectilePool(Entity owner, size_t poolSize = DEFAULT_PROJECTILES_PER_OWNER);		// initialise projectile pool for owner
	void attachSprite(Entity entity, const std::string& spriteName);							// Attach sprite to entity
	int round(float value) { return static_cast<int>(std::round(value)); }							// round helper
	// ADD COMPONENTS
	void addComponentPCTag(Entity entity) { component.players[entity] = PCTag(); }												// Set PC tag
	void addComponentNPCTag(Entity entity) { component.npcs[entity] = NPCTag(); }												// Set NPC tag
	void addComponentAmmoPickupTag(Entity entity) { component.ammoPickups[entity] = AmmoPickupTag(); }							// Set ammo pickup tag
	void addComponentHealthPickupTag(Entity entity) { component.healthPickups[entity] = HealthPickupTag(); }					// Set health pickup tag
	void addComponentTransform(Entity entity, const Vector2f& position, float scale = DEFAULT_ENTITY_SCALE, int rotation = 0, bool initialFlipH = false) { component.transforms[entity] = Transform{ position, position, scale, rotation, initialFlipH, false, position, true }; }
	void addComponentVelocity(Entity entity, float x = {}, float y = {}) { component.velocities[entity] = Velocity{ x, y }; }	// Set velocity	
	void addComponentSpeed(Entity entity, float speed = DEFAULT_UNIT_SPEED) { component.speeds[entity] = Speed{ speed }; }		// Set speed
	void addComponentCollider(Entity entity, float x, float y, int width = TILE_SIZE, int height = TILE_SIZE) { component.colliders[entity] = Collider{ SDL_Rect{ round(x), round(y), width, height } }; }	// Set collider
	void addComponentHealth(Entity entity, int currentHealth = DEFAULT_MAX_HEALTH, int maxHealth = DEFAULT_MAX_HEALTH) { component.healths[entity] = Health{ currentHealth, maxHealth }; }	// Set health
	void addComponentHealthBar(Entity entity) { component.healthBars[entity] = HealthBar(); }									// Set health bar
	void addComponentAmmo(Entity entity, int currentAmmo, int maxAmmo = DEFAULT_MAX_AMMO) { component.ammos[entity] = Ammo{ currentAmmo, maxAmmo }; }
	void addComponentDamage(Entity entity, int amount = DEFAULT_UNIT_DAMAGE) { component.damages[entity] = Damage{ amount }; }	// Set damage
	void addComponentInput(Entity entity) { component.inputs[entity] = Input{}; }												// Set input
	void addComponentDying(Entity entity) { component.dying[entity] = false; }													// Set dying state
	void addComponentIdleAnimations(Entity entity, std::string = "", std::string = "", std::string = "");						// Set idle animations
	void addComponentWalkAnimations(Entity entity, std::string = "", std::string = "", std::string = "");						// Set walk animations
	void addComponentDeathAnimations(Entity entity, std::string = "", std::string = "", std::string = "");						// Set death animations
	void addComponentAudio(Entity entity, std::string damageSound = "", std::string attackingSound = "");						// Set audio component
	void addComponentScoreValue(Entity entity, int amount) { component.scores[entity] = ScoreValue{ amount }; }					// set score value component
	void addComponentProjectileTag(Entity entity, Entity owner) { component.projectiles[entity] = ProjectileTag{ owner }; }		// Set projectile tag
	// GETTERS
	int getEntityHealth(Entity entity) { return (isValidComponent(entity, component.healths)) ? component.healths[entity].currentHealth : -1; }	// Get entity health
	int getAmmo(Entity entity) { return (isValidComponent(entity, component.ammos)) ? component.ammos[entity].currentAmmo : -1; }	// Get entity ammo
	Vector2f getEntityPosition(Entity entity) { return (isValidComponent(entity, component.transforms)) ? component.transforms[entity].position : Vector2f{}; }	// Get entity position
	int getNPCCount() const { return static_cast<int>(component.npcs.size()); };												// get current NPC count
	int getScore() const { return score; };																						// Get current score
	Vector2f MyEngineSystem::getCameraPosition() const { return cameraPosition; };												// get camera position
	SDL_Rect getEntityColliderRect(Entity entity) { return (isValidComponent(entity, component.colliders)) ? component.colliders[entity].rect : SDL_Rect{}; }	// Get entity collider rectangle
	std::string getEntityByTag(Entity entity);
};

#endif 