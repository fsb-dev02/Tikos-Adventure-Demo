#pragma once
#include <vector>
#include "entity.hpp"

static constexpr float DEFAULT_SPEED = 2.0f;

/*
Platform

A class representing moving platforms. Those are represented as a series of points, read from either a polygon or polyline
object from the Tiled editor.

*/

struct Platform_Definition;

class Platform : public Entity {
	float speed;
	int dir = 1; // Only used if loop = false. Should be always 1 or -1.
	int cur_tg = 0;
	bool loop = false;
	std::vector<SDL_FPoint> targets;

	inline bool at_target(SDL_FPoint target) const;
	inline void move_towards(float speed, SDL_FPoint dst);

public:
	Platform(const Platform_Definition& def, float speed = DEFAULT_SPEED);
	~Platform();
	void update(Level *level) override;

	void add_target(float x, float y);
	void add_target(SDL_FPoint t);
	void start();
};

enum Platform_Type {
	GRASS_2X1_THIN,
	GRASS_3X1_THIN,
	PLATFORM_TYPE_COUNT
};

struct Platform_Definition {
	int w;
	int h;
	const char* texture_path;
	int texture_offset_x;
	int texture_offset_y;
};

extern const Platform_Definition g_platform_defs[PLATFORM_TYPE_COUNT];