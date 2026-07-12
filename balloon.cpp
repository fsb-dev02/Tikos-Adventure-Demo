#include "resources.hpp"
#include "helpers.hpp"
#include "level.hpp"
#include "Balloon.hpp"
#include "global_constants.hpp"

constexpr SDL_FPoint BALLOON_SIZE = { 13, 26 };

Balloon::Balloon(SDL_FPoint pos)
	:Entity(NULL, pos, BALLOON_SIZE)
{
	const char* ANIM_PATH = "sprites/balloon/left-idle.png";
	SDL_Texture *anim_tex = g_resources.load_texture(ANIM_PATH);

	this->pos -= {size.x / 2, size.y}; // Spawn based on "foot" position
	type = BALLOON;
	anim = new Animation(anim_tex, 32, 32, 4, 2 * COMPAT_TICK_MULTIPLIER, { 10, 7 });
	state = INACTIVE;
}

Balloon::~Balloon()
{
	delete anim;
}

void Balloon::update(Level *level)
{
	const float gravity = (kgravity / 2) * PHYS_SCALE;

	const float jump = 3.0f;
	const float speed = 1.5f;

	const int tw = level->get_tile_w();
	const int th = level->get_tile_h();

	if (state == INACTIVE) {
		if (on_screen(this->get_rect(), level->cam.get_pos(), {SCREEN_WIDTH, SCREEN_HEIGHT}))
			state = ACTIVE;
		else
			return;
	}

	if (vel.x == 0.0f)
		vel.x = -speed; // Spawn pointed to the left

	vel.y += gravity;

	pos.x += vel.x * PHYS_SCALE;
	if (vel.x > 0) {
		if (test_right_collision(*level, *this)) {
			pos.x = SNAP_TO_GRID(tw, pos.x) + tw - size.x;
			vel.x = -speed;
		}
	}
	else if (vel.x < 0) {
		if (test_left_collision(*level, *this)) {
			pos.x = SNAP_TO_GRID(tw, pos.x) + tw;
			vel.x = speed;
		}
	}

	pos.y += vel.y * PHYS_SCALE;
	if (vel.y >= 0) /* not > 0, as we account for gravity */ {
		if (test_bottom_collision(*level, *this)) {
			pos.y = SNAP_TO_GRID(th, pos.y + size.y) - size.y;
			vel.y = -jump;
		}
	}
	else {
		if (test_top_collision(*level, *this)) {
			pos.y = SNAP_TO_GRID(th, pos.y) + th;
			vel.y = 0;
		}
	}
	anim->update();
}
