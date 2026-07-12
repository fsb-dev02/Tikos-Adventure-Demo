#pragma once
#include "entity.hpp"

/*
Balloon
An enemy class, derived from entity. Its behavior is quite straightforward.
Something to add to the implementation would be adding interactivity with moving platforms, as right now only the player possesses it (and the level design
deliberately avoids enemy-platform interactions)

*/

class Balloon : public Entity {
	enum State {INACTIVE, ACTIVE} state;

public:
	Balloon(SDL_FPoint pos);
	~Balloon();
	void update(Level *level) override;
	//Type get_type() { return BALLOON; }
};

