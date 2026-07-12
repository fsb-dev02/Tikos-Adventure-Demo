#include "Spring.hpp"
#include "helpers.hpp"
#include "resources.hpp"

constexpr SDL_FPoint SPRING_SIZE = { 14, 15 };

Spring::Spring(SDL_FPoint pos)
	: Entity(NULL, pos, SPRING_SIZE)
{
	const char* ANIM_PATH = "sprites/spring/bounce.png";
	SDL_Texture *anim_tex = g_resources.load_texture(ANIM_PATH);

	this->pos -= {size.x / 2, size.y - 1}; // Align to "feet"
	type = SPRING;
	anim = new Animation(anim_tex, 16, 16, 3, 16, { 2, 2 });
	state = IDLE;
}

void Spring::collide_with_obj(const Entity& e)
{
}