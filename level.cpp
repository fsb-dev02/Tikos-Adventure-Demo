#include <cmath>
#include <stdexcept>
#include <fstream>
#include <cassert>

#include "level.hpp"
#include "player.hpp"
//#include "balloon.hpp"
#include "helpers.hpp"
#include "entity_factory.hpp"

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.c"
#include "global_constants.hpp"
#include "score.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 180

extern SDL_Renderer* rend;
extern SDL_Window* win;

Level::Level(const char *filename)
{
	puts("Loading map...");
	cute_tiled_map_t* lvl = cute_tiled_load_map_from_file(filename, NULL);
	if (!lvl) {
		SDL_Log("Error loading level at path %s: %s\nError C line: %d\nError JSON line: %d",
			filename, cute_tiled_error_reason, cute_tiled_error_cline, cute_tiled_error_line);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed loading level", cute_tiled_error_reason, win);
		;
		std::exit(EXIT_FAILURE);
	}
	puts("Success");

	w = lvl->width;
	h = lvl->height;
	tile_w = lvl->tilewidth;
	tile_h = lvl->tileheight;

	// Counting tilesets and reserving space before loading.
	// If this isn't done, vector reallocations will cause unpredictable destructor calls.
	int num_tilesets = 0;
	cute_tiled_tileset_t* ts;
	for (ts = lvl->tilesets; ts; ts = ts->next)
		++num_tilesets;
	tilesets.reserve(num_tilesets);

	int cur_tileset = 0;
	for (ts = lvl->tilesets; ts; ts = ts->next){
		std::string path = "tilesets/" + std::string(filename_only(ts->image.ptr));
#if 0
		SDL_Log("Loading tileset at path %s", path.c_str());
		SDL_Log("image: %s\n", ts->image.ptr);
		SDL_Log("firstgid: %d\n", ts->firstgid);
#endif
		tilesets.emplace_back(path.c_str(), ts->tilewidth, ts->tileheight, ts->tilecount, ts->firstgid);

		for (int i = 0; i < ts->tilecount; ++i)
			ts_numerical_indexes.push_back(cur_tileset);
		++cur_tileset;
	}
	for (cute_tiled_layer_t* layer = lvl->layers; layer; layer = layer->next) {
		if (strcmp(layer->name.ptr, "background") == 0) {
			back_tiles.assign(layer->data, layer->data + layer->data_count);
			for (int& n : back_tiles)
				n = cute_tiled_unset_flags(n);
		}
		else if (strcmp(layer->name.ptr, "background_dark") == 0) {
			dark_back_tiles.assign(layer->data, layer->data + layer->data_count);
			for (int& n : dark_back_tiles)
				n = cute_tiled_unset_flags(n);
		}
		else if (strcmp(layer->name.ptr, "foreground") == 0) {
			front_tiles.assign(layer->data, layer->data + layer->data_count);
			for (int& n : front_tiles)
				n = cute_tiled_unset_flags(n);
		}
		else if (strcmp(layer->name.ptr, "collision") == 0) {
			int firstgid = 1207; // Temporary hack. This should not be hardcoded
			col_map.generate(layer->data, h, w, firstgid);
#if 0
			collision_tiles.assign(layer->data, layer->data + layer->data_count);
			for (int& n : collision_tiles)
				n = cute_tiled_unset_flags(n);
#endif
		}
		else if (strcmp(layer->type.ptr, "objectgroup") == 0 && strcmp(layer->name.ptr, "entities") == 0) {
			cute_tiled_object_t* obj;
			for (obj = layer->objects; obj; obj = obj->next) {
				for (int i = 0; i < obj->property_count; ++i){
					if (strcmp((obj->properties[i]).name.ptr, "type") == 0
						&& obj->properties[i].type == CUTE_TILED_PROPERTY_INT
						&& obj->properties[i].data.integer > (int) Entity::NONE) {
						Entity* e;
						int type = obj->properties[i].data.integer;

						if (type != Entity::Type::PLATFORM) {
							e = make_entity(type, obj->x, obj->y);
							entities.emplace_back(e);
						}
						else {
							printf("SPAWNING PLATFORM, position: %f, %f\n", obj->x, obj->y);
							assert(type == Entity::Type::PLATFORM);

							const auto default_plat = g_platform_defs[1]; // temporary
							e = new Platform(default_plat);
							Platform* p = (Platform*)e;

							float vert_x, vert_y;
							float origin_x = obj->x;
							float origin_y = obj->y;
							for (int i = 0; i < obj->vert_count * 2; i += 2) {
								vert_x = obj->vertices[i];
								vert_y = obj->vertices[i + 1];
								p->add_target(origin_x + vert_x - (default_plat.w / 2), origin_y + vert_y);
							}
							p->start();
							platforms.emplace_back(p);
						}
					}
				}
			}
		}
	}
	cute_tiled_free_map(lvl);

	for (auto& e : entities) {
		if (e->get_type() == Entity::PLAYER) {
			player = (Player*)e.get();
			goto player_found;
		}
	}
	assert(0 && "error: no player found while loading level");
player_found:
	cam = Camera(player->pos.x, player->pos.y);
	cam.set_target(player);
}

Level::~Level()
{
	if (bg)
		SDL_DestroyTexture(bg);
	while (!spawn_stack.empty()) {
		delete spawn_stack.top();
		spawn_stack.pop();
	}
}

void Level::load_bg(SDL_Renderer *rend, const char* filename)
{
    SDL_Texture* tex = IMG_LoadTexture(rend, filename);
	if (!tex) {
		SDL_Log("Could not load bg %s: %s", filename, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}
	bg = tex;
}

void Level::render()
{
	render_bg();
	render_dark_back_tiles();
	render_back_tiles();
	render_platforms();
	render_entities();
	render_front_tiles();
}

// todo
void Level::render_layer()
{

}


void Level::render_bg()
{
	const float PARALLAX = 0.5f;
	const float DRIFT_SPEED = 0.4f * PHYS_SCALE * ((int) !pause_flag); // Do not move if paused

	constexpr int scr_w = SCREEN_WIDTH;
	constexpr int scr_h = SCREEN_HEIGHT;

	if (!bg)
		return;

	static float drift = 0.0f;

	// slow automatic movement to the left
	drift -= DRIFT_SPEED;

	SDL_Point cam_pos = cam.get_pos();

	// combine camera parallax + autonomous drift
	float bg_offset = -cam_pos.x * PARALLAX + drift;

	// wrap into [-scr_w, 0]
	bg_offset = fmodf(bg_offset, (float)scr_w);
	if (bg_offset > 0)
		bg_offset -= scr_w;

	SDL_Rect dst1 = { (int)bg_offset, 0, scr_w, scr_h };
	SDL_Rect dst2 = { dst1.x + scr_w, 0, scr_w, scr_h };

	SDL_RenderCopy(rend, bg, NULL, &dst1);
	SDL_RenderCopy(rend, bg, NULL, &dst2);
}

void Level::render_back_tiles()
{
	int i, tw, th;

	tw = get_tile_w();
	th = get_tile_h();

	i = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int gid = back_tiles[i];
			SDL_Rect cur_rect = { x * tw, y * th, tw, th };
			if (gid > 0 && on_screen(cur_rect, camera, { SCREEN_WIDTH, SCREEN_HEIGHT })) {
				Tileset &ts = ts_from_gid(gid);
				SDL_Rect dst = { x * tw - camera.x, y * th - camera.y, tw, th };
				SDL_RenderCopy(rend, ts.texture, &(ts.get_tile_rect(gid)), &dst);
			}
			++i;
		}
	}
}

// Note/todo: the current method by which this function works is highly inefficient, as it
// changes the tint color for every individual texture it's drawing on every iteration of inner loop.
// Implementation of a texture atlas will be necessary to improve performance, as then only one ColorMod call is needed.
void Level::render_dark_back_tiles()
{
	int i, tw, th;

	tw = get_tile_w();
	th = get_tile_h();

	i = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int gid = dark_back_tiles[i];
			SDL_Rect cur_rect = { x * tw, y * th, tw, th };
			if (gid > 0 && on_screen(cur_rect, camera, { SCREEN_WIDTH, SCREEN_HEIGHT })) {
				Tileset& ts = ts_from_gid(gid);
				SDL_Rect dst = { x * tw - camera.x, y * th - camera.y, tw, th };
				SDL_Texture* tex = ts.texture;

				SDL_SetTextureColorMod(tex, 127, 127, 127);
				SDL_RenderCopy(rend, tex, &(ts.get_tile_rect(gid)), &dst);
				SDL_SetTextureColorMod(tex, 255, 255, 255);
			}
			++i;
		}
	}
}

void Level::render_platforms()
{
	//todo: add bounds check
	for (auto& p : platforms) {
		p->render(rend, cam.get_pos());
	}
}

void Level::render_entities()
{
	for (auto& e : entities) {
		if (on_screen(e->get_rect(), cam.get_pos(), {SCREEN_WIDTH, SCREEN_HEIGHT})) {
			e->render(rend, cam.get_pos());
		}
	}
}

void Level::render_front_tiles()
{
	int i, tw, th;

	tw = tile_w;
	th = tile_h;

	i = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int gid = front_tiles[i];
			SDL_Rect cur_rect = { x * tw, y * th, tw, th };
			if (gid > 0 && on_screen(cur_rect, camera, { SCREEN_WIDTH, SCREEN_HEIGHT })) {
				Tileset& ts = ts_from_gid(gid);
				SDL_Rect dst = { x * tw - camera.x, y * th - camera.y, tw, th };
				SDL_RenderCopy(rend, ts.texture, &(ts.get_tile_rect(gid)), &dst);
			}
			++i;
		}
	}
}

int Level::is_collidable(float x, float y) const
{
	if (x < 0 || (int) x >= w * tile_w) {
		return true;
	}
	if (y < 0 || (int) y >= h * tile_h) {
		return false;
	}

	try{
		return col_map.map.at(((int) x / COLLISION_TW) + ( (int) y / COLLISION_TH) * (w * 2));
	}
	catch (std::out_of_range) {
		SDL_Log("Out of bounds collision map vector access! Trying to index at %d\n", (((int)x / 8) + ((int)y / 8) * (w * 2)));
		return false;
	}
}

bool Level::is_platform(float x, float y) const
{
	if (x < 0 || (int)x >= w * tile_w) {
		return true;
	}
	if (y < 0 || (int)y >= h * tile_h) {
		return false;
	}

	try {
		return col_map.map.at(((int)x / COLLISION_TW) + ((int)y / COLLISION_TH) * (w * 2)) == Collision_Map::ONE_WAY;
	}
	catch (std::out_of_range) {
		SDL_Log("Out of bounds collision map vector access! Trying to index at %d\n", (((int)x / 8) + ((int)y / 8) * (w * 2)));
		return false;
	}
}


int Level::get_tile_w(void) const
{
	return tile_w;
}

int Level::get_tile_h(void) const
{
	return tile_h;
}

void Level::update()
{
	if (pause_flag)
		return;

	process_spawn_stack();
	for (auto& p : platforms) {
		p->update(this);
	}

	for (auto& e : entities) {
		assert(e != nullptr && "It's iterating over null pointers? wut?");
		e->update(this);
		if (e.get() != player && test_collision(*player, *e)) {
			//printf("Collision detected with '%s'\n", typeid(*e).name());
			player->collide_with_obj(*e);
			e->collide_with_obj(*player);
		}
	}
	// Clean up deleted objects after updating
	for (auto i = entities.begin(); i != entities.end();) {
		if ((*i)->get_type() == Entity::Type::DELETED) {
			if (cam.get_target() == i->get()) cam.set_target(NULL);
			i = entities.erase(i);
		}
		else
			++i;
	}
	cam.update(this);
	this->camera = cam.get_pos(); // Remove later. This is just for compatibility with the old camera.

	SDL_Rect player_rect = player->get_rect();
	SDL_Rect finish_line = { this->get_px_w() - TILE_WIDTH, 0, TILE_WIDTH, this->get_px_h() };
	if (!level_finished && SDL_HasIntersection(&player_rect, &finish_line)) {
		level_finished = true;
		g_score.coins = this->player->get_coins();
	}
}

void Level::spawn_entity(Entity::Type type, float x, float y)
{
	Entity* obj = make_entity(type, x, y);
	assert(obj != nullptr && "Failed spawning object! Unexpected NULL return.");

	spawn_stack.push(obj);
}

void Level::process_spawn_stack()
{
	while (!spawn_stack.empty()) {
		entities.emplace_back(spawn_stack.top());
		spawn_stack.pop();
	}
}

Tileset::Tileset(const char* filename, int tile_w, int tile_h, int num_tiles, int first_gid)
	: tile_w(tile_w), tile_h(tile_h), num_tiles(num_tiles), first_gid(first_gid)
{
	SDL_Surface* surface = IMG_Load(filename);
	if (!surface) {
		SDL_Log("Could not create surface for tileset %s: %s", filename, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}
	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
	if (!tex) {
		SDL_Log("Could not create texture for tileset %s: %s", filename, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	texture = tex;
	texture_w = surface->w;
	texture_h = surface->h;

	// Pre-generate rects for fast drawing
	tile_rects.reserve((texture_w / tile_w) * (texture_h / tile_h));
	for (int y = 0; y < texture_h / tile_h; ++y) {
		for (int x = 0; x < texture_w / tile_w; ++x) {
			SDL_Rect cur_rect = { x * tile_w, y * tile_h, tile_w, tile_h };
			tile_rects.push_back(cur_rect);
		}
	}
	SDL_FreeSurface(surface);
}

Tileset::~Tileset()
{
	SDL_Log("Tileset destructor called. firstgid for this tileset: %d\n", first_gid);
	SDL_DestroyTexture(texture);
}

void Collision_Map::generate(int* tiles, int rows, int cols, int first_gid)
{
	puts("Generating collision map.");

	// The collision map is assumed to have four "blocks" for each one block of the normal tilemap.
	map.assign(rows * cols * 4, AIR);
	printf("Resized vector to size %d. Resulting size: %d\n", rows * cols * 4, (int)map.size());

	int stride = cols * 2;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {

			// top left of current iterated map tile, translated from a 16x16 grid to an 8x8 grid
			int crow = i * 2;
			int ccol = j * 2;

			int tl = AIR;
			int tr = AIR;
			int bl = AIR;
			int br = AIR;

			switch (tiles[i * cols + j] - first_gid) {

			case NONE:
				tl = AIR; tr = AIR; bl = AIR; br = AIR;
				break;

			case FULL:
				tl = SOLID; tr = SOLID; bl = SOLID; br = SOLID;
				break;

			case TOP_ONLY:
				tl = SOLID; tr = SOLID;
				break;

			case BOTTOM_ONLY:
				bl = SOLID; br = SOLID;
				break;

			case LEFT_ONLY:
				tl = SOLID; bl = SOLID;
				break;

			case RIGHT_ONLY:
				tr = SOLID; br = SOLID;
				break;

			case TOP_LEFT:
				tl = SOLID;
				break;

			case TOP_RIGHT:
				tr = SOLID;
				break;

			case BOTTOM_LEFT:
				bl = SOLID;
				break;

			case BOTTOM_RIGHT:
				br = SOLID;
				break;

			case ONE_WAY_FULL:
				tl = ONE_WAY; tr = ONE_WAY;
				break;

			case ONE_WAY_LEFT_ONLY:
				tl = ONE_WAY;
				break;

			case ONE_WAY_RIGHT_ONLY:
				tr = ONE_WAY;
				break;

			default:
				tl = AIR; tr = AIR; bl = AIR; br = AIR;
				break;
			}

			map[crow * stride + ccol] = tl;
			map[crow * stride + ccol + 1] = tr;
			map[(crow + 1) * stride + ccol] = bl;
			map[(crow + 1) * stride + ccol + 1] = br;
		}
	}

	puts("Finished.");
}