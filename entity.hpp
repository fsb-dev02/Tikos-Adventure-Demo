#pragma once
#include "animation.hpp"
#include <SDL_rect.h>
#include <SDL_render.h>

/*
Entity

The most important of the main game classes. All objects in the map (save for tiles) derive from this.
Contains the parameters of position, size and velocity, and most importantly an animation.

*/


class Level;

class Entity {
public:
	// physical properties
	SDL_FPoint pos = { 0.0f, 0.0f };
	SDL_FPoint size = { 0.0f, 0.0f };
	SDL_FPoint vel = { 0.0f, 0.0f };

	// graphical properties
	// SDL_Point spr_size = { 0, 0 };
	// SDL_Point origin = { 0, 0 };

	//entity_type type = TYPE_NONE;

	Animation *anim;
	enum Type {DELETED = -1, NONE, PLAYER, BALLOON, COIN, SPRING, BOMB, EXPLOSION, PLATFORM} type;
	SDL_FPoint prev_pos;
	
	Entity(Animation* anim,
		SDL_FPoint pos,
		SDL_FPoint size );
	virtual ~Entity();

	//SDL_FRect get_frect(void) const { return {size.x, size.y, pos.x - (float) origin.x, pos.y - (float) origin.y}; }
	inline float left() const		{ return pos.x; }
	inline float top() const		{ return pos.y; }
	inline float right() const		{ return left() + size.x; }
	inline float bottom() const	{ return top() + size.y; }
	Type get_type() const	{ return type; }
	inline SDL_Rect get_rect() const { return{(int)pos.x, (int)pos.y, (int)size.x, (int)size.y}; };

	static constexpr float kgravity = 0.4f;
	//inline void calculate_physics();
	

	virtual void update(Level *level);
	virtual void collide_with_obj(const Entity& e) { return; } // TODO
	virtual void render(SDL_Renderer* rend, SDL_Point offset);
	void despawn();
};