#include "MyEngineSystem.h"	// Include header

MyEngineSystem::MyEngineSystem() : nextEntityId(1) {														// Start IDs from 1
#ifdef __DEBUG																								// Debug info
	debug("MyEngineSystem constructed");																	// Log construction
#endif																										// Debug info
}

MyEngineSystem::~MyEngineSystem() {																			// Destructor
#ifdef __DEBUG																								// Debug info
	debug("MyEngineSystem::~MyEngineSystem() freeing resources");											// Log destruction
#endif																										// Debug info
	sprites.clear();																						// Clear sprites
	entities.clear();																						// Clear entities
	sounds.clear();																							// Clear sounds
	groundTiles.clear();																					// Clear ground tiles
}

void MyEngineSystem::update(float deltaTime, int playerEntityId)
{
	aiSystem(component, playerEntityId, deltaTime);															// update AI
	movementSystem(component, deltaTime);																	// update movement
	collisionSystem(component, deltaTime);																	// update collisions
	updateAnimationStates(component, deltaTime);															// update animation states
	animationSystem(component, deltaTime);																	// update animations
}

void MyEngineSystem::render(std::shared_ptr<GraphicsEngine> gfx)
{
	if (!gfx) return;																						// IF NO GRAPHICS ENGINE, return
	renderTiles(gfx);																						// render background tiles first, cheap way to handle layers
	struct RenderItem { Entity entity; Sprite* sprite; Transform* transform; Animation* anim; };			// render item
	std::vector<RenderItem> list;																			// list of render items
	list.reserve(component.sprites.size());																	// reserve space
	for (auto& spriteComp : component.sprites) {															// FOR EACH SPRITE COMPONENT
		Entity entity = spriteComp.first;																	// get entity by the first part of the pair
		Sprite& sprite = spriteComp.second;																	// get sprite by the second part of the pair
		auto foundTransform = component.transforms.find(entity);											// find transform component
		if (foundTransform == component.transforms.end()) continue;											// IF NO TRANSFORM, skip
		Transform& transform = foundTransform->second;														// get transform
		Animation* anim = nullptr;																			// animation pointer
		auto foundAnim = component.animations.find(entity);													// find animation component
		if (foundAnim != component.animations.end()) anim = &foundAnim->second;								// IF FOUND ANIMATION, set pointer
		list.push_back({ entity, &sprite, &transform, anim });												// add to render list	
	}
	for (auto& rendered : list) {																			// FOR EACH RENDER ITEM
		Sprite* spritePtr = rendered.sprite;																// default sprite pointer
		int currentFrame = {};																				// zero intialise current frame
		if (rendered.anim) {																				// IF HAS ANIMATION
			auto foundSprite = sprites.find(rendered.anim->name);											// find sprite by animation name
			if (foundSprite != sprites.end()) spritePtr = &foundSprite->second;								// IF FOUND, use that sprite
			currentFrame = rendered.anim->currentFrame;														// get current frame from animation
		}
		Sprite& sprite = *spritePtr;																		// get sprite	
		int frameIndex = sprite.startFrame + currentFrame;													// calculate frame index
		int xPos = (frameIndex % (sprite.textureWidth / sprite.frameW)) * sprite.frameW;					// get X position in texture sprite sheet
		SDL_Rect src = { xPos, 0, sprite.frameW, sprite.frameH };											// source rectangle
		int posX = rendered.transform->position.x;															// world screen
		int posY = rendered.transform->position.y;															// world screen
		SDL_Rect dst = { posX, posY, sprite.frameW, sprite.frameH };										// destination rectangle
		SDL_RendererFlip flip = SDL_FLIP_NONE;																// no flip
		if (rendered.transform->flipH) flip = SDL_FLIP_HORIZONTAL;											// horizontal flip
		gfx->drawTexture(sprite.texture, &src, &dst, 0.0, nullptr, flip);									// draw texture
		posY -= sprite.textureHeight / 2; 																	// adjust posY for bar rendering
		renderHealthBar(gfx, rendered.entity, posX, posY, sprite.frameW, sprite.frameH);					// render health bar
	}
}

void MyEngineSystem::renderHealthBar(std::shared_ptr<GraphicsEngine> gfx, Entity entity, int posX, int posY, int width, int height)
{
	auto foundHealthBar = component.healthBars.find(entity);												// find health bar component
	auto foundHealth = component.healths.find(entity);														// find health component
	if (foundHealthBar == component.healthBars.end() || foundHealth == component.healths.end()) return;		// IF NO HEALTH BAR OR NO HEALTH, return
	const Health& health = foundHealth->second;																// get health
	float percent = {};																						// zero intialise percentage
	if (health.maxHealth > 0) percent = std::max(0.0f, std::min(1.0f, float(health.currentHealth) / float(health.maxHealth))); // IF MAX HEALTH > 0, calculate percentage
	int barWidth = width > 0 ? width : DEFAULT_ENTITY_WIDTH;												// bar width
	gfx->setDrawColor({ 255, 0, 0, 255 });																	// set draw color to red
	gfx->fillRect(posX, posY, barWidth, BAR_HEIGHT);				 										// draw background
	SDL_Color fgColor = { 0, 255, 0, 255 };																	// foreground color green
	gfx->setDrawColor(fgColor);																				// set draw color to foreground color
	int fillW = int(std::round(barWidth * percent));														// calculate fill width
	if (fillW > 0) gfx->fillRect(posX, posY, fillW, BAR_HEIGHT);											// IF FILL WIDTH > 0, draw health
}

void MyEngineSystem::movementSystem(Component& com, float deltaTime)
{
	for (auto& inputFound : com.inputs) {																	// FOR EACH INPUT
		Entity entity = inputFound.first;																	// get entity
		const Input& input = inputFound.second;																// get input
		float dx = input.x;																					// get input x
		float dy = input.y;																					// get input y
		float length = std::sqrt(dx * dx + dy * dy);														// calculate length
		if (length > 1.0f) { dx /= length; dy /= length; }													// normalise if length > 1
		float speed = DEFAULT_UNIT_SPEED;																	// default speed
		auto speedFound = com.speeds.find(entity);															// find speed component
		if (speedFound != com.speeds.end()) speed = speedFound->second.value;								// get speed
		com.velocities[entity] = Velocity{ dx * speed, dy * speed };										// set velocity
	}
	for (auto& velocityComp : com.velocities) {																// FOR EACH VELOCITY
		Entity entity = velocityComp.first;																	// get entity
		const Velocity& velocity = velocityComp.second;														// get velocity
		auto foundTransform = com.transforms.find(entity);													// find transform
		if (foundTransform == com.transforms.end()) continue;												// IF NO TRANSFORM, skip
		Transform& transform = foundTransform->second;														// get transform
		float attemptedX = transform.position.x + velocity.x * deltaTime;									// calculate attempted X
		float attemptedY = transform.position.y + velocity.y * deltaTime;									// calculate attempted Y
		transform.newPosition = Vector2f(attemptedX, attemptedY);											// set new position
	}
}

void MyEngineSystem::animationSystem(Component& com, float deltaTime)
{
	for (auto& foundAnim : com.animations) {																// FOR EACH ANIMATION
		Entity entity = foundAnim.first;																	// get entity
		Animation& anim = foundAnim.second;																	// get animation
		float frameDur = anim.frameDuration;																// frame duration
		anim.animTimer += deltaTime;																		// increase timer
		if (frameDur <= 0.0f) frameDur = 0.1f;																// IF FRAME DURATION <= 0, set to default
		while (anim.animTimer >= frameDur && frameDur > 0.0f) {												// WHILE TIME TO ADVANCE FRAME
			anim.animTimer -= frameDur;																		// decrease timer
			anim.currentFrame++;																			// increment frame
			if (anim.currentFrame >= anim.frameCount) 														// IF PAST LAST FRAME
				if (anim.loop) anim.currentFrame = 0;														// IF LOOPING, go to first frame
				else anim.currentFrame = anim.frameCount - 1;												// ELSE stay on last frame
		}
	}
}

void MyEngineSystem::loadSprite(const std::string& name, const std::string& filename, int frameW, int frameH, int frames, int startFrame, SDL_Color transparent)
{
	if (sprites.count(name)) return;																		// IF SPRITE ALREADY LOADED, return
	SDL_Texture* texture = ResourceManager::loadTexture(filename, transparent);								// load texture
	if (!texture) return;																					// IF FAILED TO LOAD TEXTURE, return
	int width = {}, height = {};																			// zero initialise width and height
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);											// query texture 
	Sprite sprite;																							// create Sprite
	sprite.texture = texture;																				// set texture
	sprite.frameW = frameW;																					// set frame width
	sprite.frameH = frameH;																					// set frame height
	sprite.frameCount = frames;																				// set frame count
	sprite.textureWidth = width;																			// set texture width to width which is queried
	sprite.textureHeight = height;																			// set texture height to height which is queried
	sprite.startFrame = startFrame;																			// set start frame
	sprites[name] = std::move(sprite);																		// store Sprite
}

void MyEngineSystem::attachSprite(Entity e, const std::string& spriteName)
{
	auto sprite = sprites.find(spriteName);																	// find Sprite
	if (sprite == sprites.end()) return;																	// IF SPRITE NOT FOUND, return
	component.sprites[e] = sprite->second;																	// attach sprite
	auto foundAnim = component.animations.find(e);															// find Animation
	if (foundAnim == component.animations.end()) {															// IF NO ANIMATION
		Animation anim;																						// create Animation
		anim.name = spriteName;																				// set name
		anim.loop = true;																					// set loop
		anim.frameCount = sprite->second.frameCount;														// set frame count
		anim.frameDuration = 0.1f;																			// set frame duration
		anim.animTimer = {};																				// zero initialise timer
		anim.currentFrame = {};																				// zero initialise current frame
		component.animations[e] = std::move(anim);															// store Animation
	}
	else if (foundAnim->second.name.empty()) {																// ELSE IF ANIMATION NAME EMPTY
		foundAnim->second.name = spriteName;																// set name
		foundAnim->second.frameCount = sprite->second.frameCount;											// set frame count
		foundAnim->second.animTimer = {};																	// zero initialise timer
		foundAnim->second.currentFrame = {};																// zero initialise current frame
	}
}

void MyEngineSystem::updateAnimationStates(Component& com, float deltaTime)
{
	for (auto& animStateComp : com.animationStates) {														// FOR EACH ANIMATION STATE
		Entity entity = animStateComp.first;																// get entity
		AnimationState& animState = animStateComp.second;													// get animation state
		auto foundVelocity = com.velocities.find(entity);													// find velocity
		if (foundVelocity == com.velocities.end()) continue;												// IF NO VELOCITY, skip
		const Velocity& velocity = foundVelocity->second;													// get velocity
		std::string newAnim;																				// new animation name
		if (velocity.x == 0 && velocity.y == 0) {															// IF NOT MOVING
			std::string referenceAnim = animState.previousAnimation;										// reference animation
			if (referenceAnim.empty()) { 																	// IF NO PREVIOUS ANIMATION
				auto foundAnimComp = com.animations.find(entity);											// find Animation
				if (foundAnimComp != com.animations.end())													// has Animation
					referenceAnim = foundAnimComp->second.name;												// set reference animation
			}
			if (referenceAnim.find("_up") != std::string::npos)												// IF UP
				newAnim = animState.idle_up;																// set idle up
			else if (referenceAnim.find("_right") != std::string::npos)										// IF RIGHT
				newAnim = animState.idle_right;																// set idle right
			else if (referenceAnim.find("_down") != std::string::npos)										// IF DOWN
				newAnim = animState.idle_down;																// set idle down
			else newAnim = animState.idle_down;																// default to idle down
		}
		else {																								// ELSE IF MOVING
			if (std::abs(velocity.x) > std::abs(velocity.y)) {												// IF HORIZONTAL MOVEMENT DOMINANT
				if (velocity.x > 0) {																		// IF MOVING RIGHT
					newAnim = animState.walk_right;															// use right animation
					auto foundTransform = com.transforms.find(entity);										// find Transform
					if (foundTransform != com.transforms.end())												// IF FOUND
						if (!foundTransform->second.initialFlipH) foundTransform->second.flipH = false;		// IF NO INITIAL FLIP, no flip
						else foundTransform->second.flipH = true;											// ELSE INTIAL FLIP, flip
				}
				else {																						// ELSE MOVING LEFT
					newAnim = animState.walk_right;															// use right animation
					auto foundTransform = com.transforms.find(entity);										// find Transform
					if (foundTransform != com.transforms.end())												// IF FOUND
						if (!foundTransform->second.initialFlipH) foundTransform->second.flipH = true;		// IF INITIAL FLIP, flip
						else foundTransform->second.flipH = false;											// ELSE NO INITIAL FLIP, no flip
				}
			}
			else {																							// ELSE VERTICAL MOVEMENT DOMINANT
				auto foundTransform = com.transforms.find(entity);											// find Transform
				if (velocity.y > 0) newAnim = animState.walk_down;											// IF MOVING DOWN - use down animation
				else newAnim = animState.walk_up;															// ELSE MOVING UP - use up animation
			}
			animState.previousAnimation = newAnim;															// store previous animation
		}
		auto foundAnim = com.animations.find(entity);														// find Animation
		if (foundAnim != com.animations.end()) {															// IF HAS ANIMATION
			if (foundAnim->second.name != newAnim) {														// IF ANIMATION CHANGED
				foundAnim->second.name = newAnim;															// set new animation name
				foundAnim->second.currentFrame = 0;															// reset current frame
				foundAnim->second.animTimer = 0.0f;															// reset timer
				auto foundSprite = sprites.find(newAnim);													// find Sprite by animation name
				if (foundSprite != sprites.end()) {															// IF SPRITE FOUND
					foundAnim->second.frameCount = foundSprite->second.frameCount;							// set frame count from sprite
					foundAnim->second.frameDuration = 0.1f;													// set frame duration
				}
				else {																						// ELSE SPRITE NOT FOUND
					foundAnim->second.frameCount = 1;														// set frame count to 1
					foundAnim->second.frameDuration = 0.1f;													// set frame duration
				}
			}
		}
	}
}

void MyEngineSystem::setEntityInput(Entity e, float x, float y)
{
	if (x < -1.0f) x = -1.0f; else if (x > 1.0f) x = 1.0f;													// clamp input
	if (y < -1.0f) y = -1.0f; else if (y > 1.0f) y = 1.0f;													// clamp input
	Input input;																							// create input
	input.x = x;																							// set x
	input.y = y;																							// set y
	component.inputs[e] = std::move(input);																	// store input
}

void MyEngineSystem::addGroundTile(const std::string& spriteName, int x, int y)
{
	Tile tile;																								// create tile
	tile.x = x; tile.y = y; tile.spriteName = spriteName;													// set tile properties
	groundTiles.push_back(std::move(tile));																	// add tile to ground tiles
}

void MyEngineSystem::renderTiles(std::shared_ptr<GraphicsEngine> gfx) {
	for (const Tile& tile : groundTiles) {																	// FOR EACH TILE
		auto foundSprite = sprites.find(tile.spriteName);													// find Sprite
		if (foundSprite == sprites.end()) continue;															// IF SPRITE NOT FOUND, skip
		const Sprite& sprite = foundSprite->second;															// get Sprite
		SDL_Rect src = { 0, 0, sprite.frameW, sprite.frameH };												// source rectangle
		SDL_Rect dst = { tile.x, tile.y, sprite.frameW, sprite.frameH };									// destination rectangle
		gfx->drawTexture(sprite.texture, &src, &dst, 0.0, nullptr, SDL_FLIP_NONE);							// draw texture
	}
}

void MyEngineSystem::collisionSystem(Component& com, float deltaTime)
{
	for (auto& velocityComp : com.velocities) {																// FOR EACH VELOCITY
		Entity entity = velocityComp.first;																	// get entity
		Velocity velocity = velocityComp.second;															// get velocity
		auto foundTransform = com.transforms.find(entity);													// find transform
		if (foundTransform == com.transforms.end()) continue;												// IF NO TRANSFORM, skip
		Transform& transform = foundTransform->second;														// get transform
		int colliderWidth = DEFAULT_ENTITY_WIDTH, colliderHeight = DEFAULT_ENTITY_HEIGHT;					// default collider size
		auto colliderFound = com.colliders.find(entity);													// find collider
		if (colliderFound != com.colliders.end()) { colliderWidth = colliderFound->second.width; colliderHeight = colliderFound->second.height; }	// get collider size
		float attemptedX = transform.newPosition.x;															// attempted X
		float attemptedY = transform.newPosition.y;															// attempted Y
		std::vector<SDL_Rect> obstacles;																	// list of obstacles
		obstacles.reserve(com.colliders.size());															// reserve space
		for (const auto& collider : com.colliders) {														// FOR EACH COLLIDER
			Entity other = collider.first;																	// get other entity
			if (other == entity) continue; 																	// skip self
			auto foundOtherTransform = com.transforms.find(other);											// find other transform
			if (foundOtherTransform == com.transforms.end()) continue;										// IF NO TRANSFORM, skip
			const Transform& otherTransform = foundOtherTransform->second;									// get other transform
			const Collider& otherCollider = collider.second;												// get other collider
			SDL_Rect otherRect = { int(std::floor(otherTransform.position.x)), int(std::floor(otherTransform.position.y)), otherCollider.width, otherCollider.height };	// other rectangle
			obstacles.push_back(otherRect); 																// add to obstacles
		}
		SDL_Rect rectX = { int(std::floor(attemptedX)), int(std::floor(transform.position.y)), colliderWidth, colliderHeight };	// rectangle for X movement
		for (const SDL_Rect& ob : obstacles) {																// FOR EACH OBSTACLE
			if (SDL_HasIntersection(&rectX, &ob) == SDL_TRUE) {												// IF INTERSECTS
				if (velocity.x > 0.0f) attemptedX = float(ob.x - rectX.w);									// IF MOVING RIGHT, place left of obstacle
				else attemptedX = float(ob.x + ob.w);														// ELSE MOVING LEFT, place right of obstacle
				velocity.x = 0.0f;																			// stop horizontal movement
				rectX.x = int(std::floor(attemptedX));														// update rectX
				break;																						// exit obstacle loop
			}
		}
		transform.position.x = attemptedX;																	// update position X
		transform.newPosition.x = attemptedX;																// keep newPosition
		SDL_Rect rectY = { int(std::floor(transform.position.x)), int(std::floor(attemptedY)), colliderWidth, colliderHeight };	// rectangle for Y movement
		for (const SDL_Rect& obstacle : obstacles) {														// FOR EACH OBSTACLE
			if (SDL_HasIntersection(&rectY, &obstacle) == SDL_TRUE) {										// IF INTERSECTS
				if (velocity.y > 0.0f) attemptedY = float(obstacle.y - rectY.h);							// IF MOVING DOWN, place above obstacle
				else attemptedY = float(obstacle.y + obstacle.h);											// ELSE MOVING UP, place below obstacle
				velocity.y = 0.0f;																			// stop vertical movement
				rectY.y = int(std::floor(attemptedY));														// update rectY
				break;																						// exit obstacle loop
			}
		}
		transform.position.y = attemptedY;																	// update position Y
		transform.newPosition.y = attemptedY;																// keep newPosition
		com.velocities[entity] = velocity;																	// update velocity
	}
}


void MyEngineSystem::aiSystem(Component& com, Entity playerEntity, float deltaTime)
{
	if (playerEntity == 0) return;																			// IF NO PLAYER ENTITY, return
	auto foundPCTransform = com.transforms.find(playerEntity);												// find player transform
	if (foundPCTransform == com.transforms.end()) return;													// IF NO PLAYER TRANSFORM, return
	const Transform& pcTransform = foundPCTransform->second;												// get player transform
	for (auto& npcEntry : com.npcs) {																		// FOR EACH NPC
		Entity entity = npcEntry.first;																		// get entity
		auto transformFound = com.transforms.find(entity);													// find transform						
		if (transformFound == com.transforms.end()) continue;												// IF NO TRANSFORM, skip
		Transform& npcTransform = transformFound->second;													// get transform
		float dx = pcTransform.position.x - npcTransform.position.x;										// delta x
		float dy = pcTransform.position.y - npcTransform.position.y;										// delta y
		float distance = dx * dx + dy * dy;																	// distance squared
		if (distance <= DEFAULT_NPC_CHASE_RANGE) {															// IF WITHIN CHASE RANGE
			float distSqrt = std::sqrt(distance);															// distance
			float newVelX = dx / distSqrt;																	// normalised x
			float newVelY = dy / distSqrt;																	// normalised y
			com.inputs[entity] = Input{ newVelX, newVelY };													// set input to move towards player
		}
		else com.inputs[entity] = Input{ 0.0f, 0.0f };														// ELSE stop movement
	}
}

int MyEngineSystem::getEntityHealth(Entity entity) {
	auto foundHealth = component.healths.find(entity);														// find health component
	if (foundHealth != component.healths.end()) {															// IF FOUND
		return foundHealth->second.currentHealth;															// return current health
	}
	return -1;																								// not found
}

void MyEngineSystem::loadSound(const std::string& name, const std::string& filename) {
	if (sounds.count(name)) return;																			// already loaded
	Mix_Chunk* chunk = ResourceManager::loadSound(filename);												// load sound
	if (!chunk) return;																						// failed to load
	sounds[name] = chunk;																					// store sound
	std::cout << "Loaded sound: " << name << std::endl;														// log loaded sound
}

MyEngineSystem::Entity MyEngineSystem::spawnPC(float x, float y, float scale)
{
	Entity entity = createEntity();																			// create entity
	component.players[entity] = PCTag();																	// add PC tag
	component.transforms[entity] = Transform{ Vector2f(x, y), 0, false };									// add transform
	component.velocities[entity] = Velocity{ 0,0 };															// add velocity
	component.speeds[entity] = Speed{ DEFAULT_PC_SPEED };													// add speed
	component.colliders[entity] = Collider{ DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT };					// add collider
	component.healths[entity] = Health{ 100, 100 };															// add health
	component.healthBars[entity] = HealthBar();																// add health bar
	component.ammos[entity] = Ammo{ 30, 50 };																// add ammo
	component.damages[entity] = Damage{ 10 };																// add damage
	component.inputs[entity] = Input{ 0.0f, 0.0f };															// add input
	AnimationState animState;																				// create animation state
	animState.idle_down = "player_idle_down";																// set idle down
	animState.idle_right = "player_idle_right";																// set idle left
	animState.idle_up = "player_idle_up";																	// set idle up
	animState.walk_down = "player_walk_down";																// set walk up
	animState.walk_right = "player_walk_right";																// set walk right
	animState.walk_up = "player_walk_up";																	// set walk up
	animState.death_down = "player_death_down";																// set death down
	animState.death_right = "player_death_right";															// set death right
	animState.death_up = "player_death_up";																	// set death up
	component.animationStates[entity] = std::move(animState);												// store animation state
	std::string startAnim = component.animationStates[entity].idle_down;									// starting animation 
	attachSprite(entity, startAnim);																		// attach default sprite
	return entity;																							// return entity
}

MyEngineSystem::Entity MyEngineSystem::spawnNPC(float x, float y, float scale) {
	Entity entity = createEntity();																			// create entity
	component.npcs[entity] = NPCTag();																		// add NPC tag
	component.transforms[entity] = Transform{ Vector2f(x, y), 0, true };									// add transform
	component.velocities[entity] = Velocity{ 0,0 };															// add velocity
	component.speeds[entity] = Speed{ DEFAULT_UNIT_SPEED };													// add speed
	component.colliders[entity] = Collider{ DEFAULT_ENTITY_WIDTH, DEFAULT_ENTITY_HEIGHT };					// add collider
	component.healths[entity] = Health{ 100, 100 };															// add health
	component.healthBars[entity] = HealthBar();																// add health bar
	component.damages[entity] = Damage{ 10 };																// add damage
	AnimationState animState;																				// create animation state
	animState.idle_down = "zombie_idle_down";																// set idle down
	animState.idle_right = "zombie_idle_right";																// set idle left
	animState.idle_up = "zombie_idle_up";																	// set idle up
	animState.walk_down = "zombie_walk_down";																// set walk up
	animState.walk_right = "zombie_walk_right";																// set walk right
	animState.walk_up = "zombie_walk_up";																	// set walk up
	animState.death_down = "zombie_death_down";																// set death down
	animState.death_right = "zombie_death_right";															// set death right
	animState.death_up = "zombie_death_up";																	// set death up
	component.animationStates[entity] = std::move(animState);												// store animation state
	std::string startAnim = component.animationStates[entity].idle_down;									// starting animation 
	attachSprite(entity, startAnim);																		// attach default sprite
	return entity;																							// return entity
}

MyEngineSystem::Entity MyEngineSystem::spawnBlock(float x, float y, float scale)
{
	Entity entity = createEntity();																			// create entity
	attachSprite(entity, "block");																			// attach block sprite
	auto foundSprite = sprites.find("block");																// find block sprite
	int width = DEFAULT_ENTITY_WIDTH, height = DEFAULT_ENTITY_HEIGHT;										// default dimensions
	if (foundSprite != sprites.end()) {																		// IF SPRITE FOUND
		width = foundSprite->second.frameW;																	// get width
		height = foundSprite->second.frameH;																// get height
	}
	component.transforms[entity] = Transform{ Vector2f(x, y) };												// set transform
	component.colliders[entity] = Collider{ width, height };												// set collider
	return entity;																							// return entity
}