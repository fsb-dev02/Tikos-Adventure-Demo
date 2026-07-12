#include "explosion.hpp"
#include "resources.hpp"
#include "global_constants.hpp"
#include <cassert>

static constexpr SDL_FPoint EXPLOSION_SIZE = { 24, 24 };

Explosion::Explosion(SDL_FPoint pos) :Entity(anim, pos, EXPLOSION_SIZE)
{
	type = Entity::Type::EXPLOSION;

	SDL_Texture* anim_tex = g_resources.load_texture("sprites/explosion.png");

	anim = new Animation(anim_tex, 32, 32, 6, 4 * COMPAT_TICK_MULTIPLIER, {4, 4}, {0,0});
	assert(anim);
	anim->set_loop(false);
}

Explosion::~Explosion()
{
	delete anim;
}

void Explosion::update(Level *level)
{
	anim->update();
	if (anim->is_over()) {
		this->type = DELETED;
	}
}

