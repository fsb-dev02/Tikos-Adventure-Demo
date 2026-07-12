#include <cstdio>
#include <fstream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <cassert>
#include <typeinfo>
#include <string>

#include "game.hpp"
#include "animation.hpp"
#include "entity.hpp"
#include "input.hpp"
#include "level.hpp"
#include "hud.hpp"
#include "camera.hpp"

#include "player.hpp"
#include "balloon.hpp"
#include "coin.hpp"
#include "Spring.hpp"
#include "bomb.hpp"

#include "helpers.hpp"
#include "global_constants.hpp"


//Player* g_player = NULL;
//Level* g_level;

int parse_csv(std::vector<int>& tilemap, const char* file);

#if 0
void game_loop(void)
{
	constexpr int scr_w = 320;
	constexpr int scr_h = 180;
	int ticker = 0;
	const char* bg_image = "bluesky.bmp";

	begin:

	Level level("levels/rock_mnt.tmj");
	level.load_bg(rend, bg_image);
	g_level = &level;

#if 0
	//let's populate the entity array
	const char* entities_path = "levels/lvl3/level1_entities.csv";
	auto *temp = new std::vector<int>;
	if (parse_csv(*temp, entities_path) == -1)
		assert(0 && "there was an error with the entities csv");
	int i = 0;
	for (int &n : *temp) {
		enum {NONE = -1, PLAYER, BALLOON, COIN, SPRING};
		if (n != NONE) {
			SDL_FPoint pos = { (float)(i % g_level->w) * g_level->get_tile_w(),
								(float)(i / g_level->w) * g_level->get_tile_h() };
			printf("Spawning entity at %d, %d : ", (int) pos.x, (int) pos.y);
			switch (n) {
			case PLAYER:
				puts("player");
				if (!g_player)
					g_player = new Player(pos);
				break;
			case BALLOON:
				puts("balloon");
				entities.emplace_back(new Balloon(pos));
				break;
			case COIN:
				puts("coin");
				entities.emplace_back(new Coin(pos));
				break;
			case SPRING:
				puts("spring");
				entities.emplace_back(new Spring(pos));
				break;
			}
		}
		++i;
	}
	delete temp;
	assert(g_player && "Invalid map. There is no player.");

	Player& player = *g_player;
#endif

	Text_Renderer text;

	bool quit = false;

	while (!quit) {

#ifdef _DEBUG
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R]) {
			SDL_ResetKeyboard();
			SDL_Delay(300);
			g_level->restart();
		}
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LCTRL] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_B]) {
			SDL_ResetKeyboard();
			g_level->spawn_entity(Entity::Type::BOMB, g_player->pos.x + 32, g_player->pos.y - 32);
		}
#endif
		// This could probably be better written.
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RETURN]) {
			SDL_ResetKeyboard();
			g_level->toggle_pause();
		}

		if (g_level->should_reset()) {
			g_level->toggle_reset();
			goto begin;
		}

		process_input();
		if (g_input.quit == true)
			quit = true;

		g_level->update();

		SDL_RenderClear(rend);

		g_level->render();
		if (g_level->is_paused()) {
			const int pause_offset = 20; // for rendering
			text.render_text("PAUSE", { SCREEN_WIDTH / 2 - pause_offset, SCREEN_HEIGHT / 2 });
		}
		text.render_text(std::to_string(g_player->get_coins()), { 8, 8 });

		SDL_RenderPresent(rend);
		SDL_Delay(1000 / FRAMERATE);
	}
}
#endif

#if 0
SDL_Texture* load_texture(const char* path)
{
	SDL_Texture* texture = IMG_LoadTexture(rend, path);
	if (!texture) {
		SDL_Log("Error creating texture from file %s: %s", path, SDL_GetError());
	}
	return texture;
}

// loads tilemap info from a Tiled CSV into an int vector
int parse_csv(std::vector<int>& v, const char* filename)
{
	puts("opening CSV...");
	std::ifstream f(filename);
	if (!f) return -1;

	// this loop is extremely hacky - should probably be rewritten later
	puts("Reading CSV...");
	int n;
	while (f >> n) {
		if (f.get() != ',') f.unget();
		v.push_back(n);
	}
	return 0;
}
#endif