#pragma once
#include "entity.hpp"
#include "level.hpp"
#include <cmath>
#include <SDL_rect.h>

inline SDL_FPoint operator+(SDL_FPoint a, SDL_FPoint b) {
	return { a.x + b.x, a.y + b.y };
}
inline SDL_FPoint operator-(SDL_FPoint a, SDL_FPoint b) {
	return { a.x - b.x, a.y - b.y };
}
inline SDL_FPoint& operator+=(SDL_FPoint& a, const SDL_FPoint& b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline SDL_FPoint& operator-=(SDL_FPoint& a, const SDL_FPoint& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
template<typename T>
inline SDL_FPoint operator*(SDL_FPoint p, T s) {
	return { p.x * s, p.y * s };
}
template<typename T>
inline SDL_FPoint operator/(SDL_FPoint p, T s) {
	return { p.x / s, p.y / s };
}


// Truncates a position towards the previous multiple of tile_size.
// Update: truncates towards half the tile size as per the 8x8 collision system implementation, half of the 16x16 tiles.
inline static constexpr float SNAP_TO_GRID(int tile_size, float pos)
{
	return pos < 0 ? -tile_size : pos - ((int)pos % (tile_size));
}

inline static bool test_collision(const Entity& e1, const Entity& e2)
{
	return (e1.right() > e2.left() && e1.left() < e2.right()) && (e1.bottom() > e2.top() && e1.top() < e2.bottom());
}

inline static constexpr bool on_screen(SDL_Rect rect, SDL_Point cam, SDL_Point scr_size)
{
	return rect.x + rect.w > cam.x && rect.y + rect.h > cam.y
			&& rect.x < cam.x + scr_size.x && rect.y < cam.y + scr_size.y;
}

inline constexpr float square(float x) { return x * x; }

// basic pythagorean theorem
inline float distance(SDL_FPoint a, SDL_FPoint b)
{
	return sqrt(square(fabsf(a.x - b.x)) + square(fabsf(a.y - b.y)));
}

// Takes a file path, trims everything behind it and leaves only the filename.
inline static const char* filename_only(const char* s)
{
	int i = 0;
	while (*s) {
		++s;
		++i;
	}
	while (*s != '/' && i > 0) {
		--s;
		--i;
	}
	return s + (i > 0);
}

inline int test_bottom_collision(Level& lvl, Entity& e)
{
	return lvl.is_collidable(e.left() + 1, e.bottom()) | lvl.is_collidable(e.right() - 1, e.bottom());
}

inline int test_top_collision(Level& lvl, Entity& e)
{
	return lvl.is_collidable(e.left() + 1, e.top()) == Collision_Map::SOLID
		|| lvl.is_collidable(e.right() - 1, e.top()) == Collision_Map::SOLID;
}

inline int test_left_collision(Level& lvl, Entity& e)
{
	return lvl.is_collidable(e.left(), e.top() + 1) == Collision_Map::SOLID 
		|| lvl.is_collidable(e.left(), e.bottom() - 1) == Collision_Map::SOLID;
}

inline int test_right_collision(Level& lvl, Entity& e)
{
	return lvl.is_collidable(e.right(), e.top() + 1) == Collision_Map::SOLID
		|| lvl.is_collidable(e.right(), e.bottom() - 1) == Collision_Map::SOLID;
}

inline int test_x_overlap(const Entity& a, const Entity& b) {
	return a.right() > b.left() && a.left() < b.right();
}

inline int test_y_overlap(const Entity& a, const Entity& b) {
	return a.bottom() > b.top() && a.top() < b.bottom();
}

inline SDL_FPoint adjust_to_feet(Entity& e)
{
	SDL_FPoint ret = e.pos;
	ret.x -= e.size.x / 2;
	ret.y -= e.size.y;
	return ret;
}