#pragma once
#include "entity.hpp"
#include "player.hpp"
#include "balloon.hpp"
#include "coin.hpp"
#include "Spring.hpp"
#include "bomb.hpp"
#include "explosion.hpp"
#include "platform.hpp"


// Returned pointers should be deallocated by whoever calls this function.
inline Entity* make_entity(int n, float x, float y)
{

	auto type = (Entity::Type) n;
	SDL_FPoint pos = { x, y };
	Entity* ret;

	switch (type) {
	case Entity::Type::PLAYER:
		ret = new Player(pos);
		break;

	case Entity::Type::BALLOON:
		ret = new Balloon(pos);
		break;

	case Entity::Type::COIN:
		ret = new Coin(pos);
		break;

	case Entity::Type::SPRING:
		ret = new Spring(pos);
		break;

	case Entity::Type::BOMB:
		ret = new Bomb(pos);
		break;

	case Entity::Type::EXPLOSION:
		ret = new Explosion(pos);
		break;

	default:
		SDL_Log("Warning: make_entity failure! Invalid type %d passed in.", (int) type);
		return NULL;
	}
	if (!ret)
		SDL_Log("Warning: make_entity failure, new operator for type id %d returned NULL.", (int) type);
	return ret;
}