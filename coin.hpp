#pragma once
#include "entity.hpp"
#include "sound.hpp"

/*
Coin

A collectable. Destroys itself on impact.

*/

class Coin : public Entity {
public:
	Mix_Chunk* sfx_pickup;

	enum State {ACTIVE, DYING, DEAD} state = ACTIVE;

	Coin(SDL_FPoint pos);
	~Coin();

	void update(Level *level) override;
	void collide_with_obj(const Entity& e) override;
	void kill();

	enum Coin::State get_state() const { return state; }
	
	//Type get_type() { return COIN; }
};

