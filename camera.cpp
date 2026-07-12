#include "camera.hpp"
#include "level.hpp"
#include "global_constants.hpp"


void Camera::set_target(const Entity *e)
{
	target = e;
}

void Camera::set_pos(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Camera::update(Level *level)
{
	const int scr_w = 320;
	const int scr_h = 180;

	// Follow target
	if (!target)
		return;
	if (target->get_type() == Entity::PLAYER && ((Player*)target)->is_dead())
		return;
	SDL_Point target_pos = { (int)target->pos.x - scr_w / 2, (int)target->pos.y - scr_h / 2 };

	x = target_pos.x;
	if (y < target_pos.y) {
		y += follow_speed * PHYS_SCALE;
		if (y > target_pos.y)
			y = target_pos.y;
	}
	else if (y > target_pos.y) {
		y -= follow_speed * PHYS_SCALE;
		if (y < target_pos.y)
			y = target_pos.y;
	}

	// Stay within level boundaries
	const int xlimit = level->get_px_w() - scr_w;
	const int ylimit = level->get_px_h() - scr_h;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > xlimit)
		x = xlimit;
	if (y > ylimit)
		y = ylimit;

}