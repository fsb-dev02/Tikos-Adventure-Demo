#pragma once
#include "entity.hpp"
#include "sound.hpp"

/*
Camera

Just like Balloon, but with far more complex enemy AI. Will stay idle until the player approaches and try to jump towards you,
detonating when it believes it's near enough (destroying itself and spawning an explosion object).

*/

class Bomb : public Entity
{
	enum State {IDLE, AGGRO, JUMPING, EXPLODING} state;
	enum {RIGHT = 1, LEFT = -1} direction;
	bool on_ground = false;

	Animation* anim_idle;
	Animation* anim_aggro;
	Animation* anim_explosion;
	Mix_Chunk* sfx_blast;

public:
	Bomb(SDL_FPoint pos);
	~Bomb();

	void render(SDL_Renderer *rend, SDL_Point offset) override;
	void update(Level *level) override;
	void collide_with_obj(const Entity& e) override;
	inline void detonate();
	inline void aggro();
	inline void jump(float n);
};

