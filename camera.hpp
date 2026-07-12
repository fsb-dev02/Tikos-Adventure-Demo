#pragma once
#include "entity.hpp"
#include "player.hpp"
#include <cmath>

/*
Camera

A class to be used side-by-side with Level. Determines where map objects should be shown on the screen at any given time.

*/

class Camera
{
	float x;
	float y;
	float follow_speed = 2.0f;
	const Entity *target;
	//enum Target_Type {NONE, POINT, ENTITY} target_type;

public:
	Camera(int x = 0, int y = 0) : x(x), y(y), target(NULL) {}

	void update(Level *level);
	void set_pos(int x, int y);
	void set_target(const Entity*);
	void set_speed(float n) { follow_speed = n; }
	const Entity *get_target() const { return target; }
	const SDL_Point get_pos() const { return { (int) x, (int) y }; }
};

