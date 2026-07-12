#include <cmath>
#include <cassert>
#include "entity.hpp"
#include "helpers.hpp"

constexpr int TILE = 16;

Entity::Entity(Animation* anim = NULL,
	SDL_FPoint pos = {0.0f, 0.0f},
	SDL_FPoint size = {0.0f, 0.0f})
	: anim(anim), pos(pos), size(size), type(NONE)
{
}

Entity::~Entity()
{

}

void Entity::update(Level *level)
{
	anim->update();
}

void Entity::render(SDL_Renderer* rend, SDL_Point offset = {0, 0})
{
	if (!anim)
		return;
	const SDL_Rect dst = { (int) pos.x - offset.x, (int) pos.y - offset.y, size.x, size.y };
	anim->render(rend, &dst);
}

void Entity::despawn()
{
	this->type = DELETED;
	return;
}
