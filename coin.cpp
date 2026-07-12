#include "resources.hpp"
#include "helpers.hpp"
#include "coin.hpp"
#include "global_constants.hpp"

constexpr SDL_FPoint COIN_SIZE = { 10, 10 };

Coin::Coin(SDL_FPoint pos)
	:Entity(anim, pos, COIN_SIZE)
{
	type = COIN;

	SDL_Texture *anim_tex = g_resources.load_texture("sprites/coin/coin_.png");
	anim = new Animation(anim_tex, 16, 16, 12, 10 * COMPAT_TICK_MULTIPLIER, { 4, 4 });

	this->pos = adjust_to_feet(*this);

	sfx_pickup = g_resources.load_sfx("sounds/coin_pickup.wav");
}

void Coin::update(Level *level)
{
	switch(state) {
	case ACTIVE:
		break;
	case DYING:
		vel.y += kgravity * PHYS_SCALE;
		pos.y += vel.y * PHYS_SCALE;
		if (vel.y > 0)
			state = DEAD;
		break;
	case DEAD:
		type = DELETED;
		break;
	}
	anim->update();
}

void Coin::collide_with_obj(const Entity& e)
{
	switch (e.get_type()) {
	case Entity::PLAYER:
		if (state == ACTIVE && ((Player*) (&e))->is_dead() == false)
			this->kill();
	default:
		break;
	}
}

Coin::~Coin()
{
	delete anim;
}

void Coin::kill()
{
	const float jump = 3.0f;

	vel.y = -jump;
	state = DYING;
	play_sound(sfx_pickup, 2);
}