#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <memory>

#include "entity.hpp"
#include "camera.hpp"
#include "platform.hpp"

#include "cute_tiled.c"
#include <stack>

/*
Level

As of now, this is the most important and largest class. Handles the entirety of the world simulation, including
tilemaps, tilesets, the collision map and all entities currently in play.

Levels are created by and use the format of the Tiled v1.9 editor.
For level editing, it is recommended to use "sprites/visual_hitboxes.png" as a guide for working with the collision map.

*/

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

class Player;

class Tileset {
	friend class Level;

	SDL_Texture* texture;
	std::vector<SDL_Rect> tile_rects;
	int texture_w; // in pixels!
	int texture_h;
	const int tile_w;
	const int tile_h;

public:
	const int first_gid;
	const int num_tiles;
	Tileset(const char *filename, int tile_w, int tile_h, int num_tiles, int first_gid = 0);
	~Tileset();
	SDL_Rect &get_tile_rect(int gid) { return tile_rects.at(gid - first_gid); }
};

class Collision_Map {
	friend class Level;
	std::vector<uint8_t> map;

public:
	// Only used for generating, not for storage in the actual map.
	enum Temp_Collision_Type { NONE = 0, FULL,
		TOP_ONLY, BOTTOM_ONLY, LEFT_ONLY, RIGHT_ONLY,
		TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT,
		ONE_WAY_FULL, ONE_WAY_LEFT_ONLY, ONE_WAY_RIGHT_ONLY
	};
	enum Collision_Type { AIR, SOLID, ONE_WAY};
	void generate(int* tiles, int rows, int cols, int first_gid = 0);
};

class Level {
	SDL_Texture* bg = NULL;

	std::vector<int> back_tiles;
	std::vector<int> dark_back_tiles;
	std::vector<int> front_tiles;
	std::vector<int> collision_tiles;
	std::vector<Tileset> tilesets;
	Collision_Map col_map;


	// for instant retrieval of the appropriate Tileset's position in memory from any given GID
	std::vector<int> ts_numerical_indexes;

	// Player is also an entity stored in the entity vector. This is to be passed for enemy detection, etc
	Player* player;

	bool pause_flag = false;
	bool reset_flag = false;
	bool level_finished = false;

public:
	std::vector<std::unique_ptr<Entity>> entities;
	std::vector<std::unique_ptr<Platform>> platforms;
	std::stack<Entity*> spawn_stack;

	int tile_w;
	int tile_h;

	// Both of these measurements are in tiles.
	int w;
	int h;

	Level(const char* filename);
	~Level();
	void load_bg(SDL_Renderer *rend, const char* filename);
	void render();
	void render_layer();
	void render_bg();
	void render_back_tiles();
	void render_dark_back_tiles();
	void render_platforms();
	void render_entities();
	void render_front_tiles();
	void process_spawn_stack();

	inline Tileset& ts_from_gid(int gid) { return tilesets.at(ts_numerical_indexes.at(gid)); }
	int is_collidable(float x, float y) const;
	bool is_platform(float x, float y) const;
	int get_tile_w(void) const;
	int get_tile_h(void) const;
	inline int get_px_w(void) const { return this->w * tile_w; }
	inline int get_px_h(void) const { return this->h * tile_h; }

	void update();
	void spawn_entity(enum Entity::Type type, float x, float y);
	inline Player* get_player() { return player; }
	inline void restart() { reset_flag = true; }
	inline bool should_reset() const { return reset_flag; }
	inline void toggle_reset() { reset_flag = !reset_flag; }
	inline bool is_paused() const { return pause_flag; }
	inline void toggle_pause() { pause_flag = !pause_flag;  }
	inline bool is_finished() const{ return level_finished; }

	Camera cam;
	SDL_Point camera = { 0, 0 };
};

