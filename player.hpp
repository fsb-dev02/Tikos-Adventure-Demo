#pragma once
#include "entity.hpp"

/*
Player

Derived from entity, handles the main character, its physics and its controls.
*/

class Level;
class Player : public Entity {
	friend class Level;

	enum State {ACTIVE, DYING, DEAD, DELETED} state;
	int coins = 0;
	bool on_ground = false;
	bool jumping = false;
	int jump_ticker = 0;
	Entity* cur_platform = NULL;

public:
	Player(SDL_FPoint pos);
	~Player();
	void update(Level *level) override;
	void collide_with_obj(const Entity& e) override;
	bool test_platform_collision(const Entity& platform);

	inline enum Type get_type() const { return PLAYER; }
	inline bool is_dead() const { return state == DYING || state == DEAD || state == DELETED; }
	inline int get_coins() const { return coins; }

	void bounce(float target_vel);
	void kill();
};

