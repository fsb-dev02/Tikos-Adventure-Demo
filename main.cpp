#include <cstdio>
#include <cassert>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "game.hpp"
#include "state.hpp"
#include "input.hpp"
#include "global_constants.hpp"


SDL_Window* win;
SDL_Renderer* rend;

enum {MENU, GAME, QUIT} g_app_state;

static bool init(void)
{
	puts("initializing SDL...");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failure", SDL_GetError(), NULL);
		SDL_Quit();
		return false;
	}
	win = SDL_CreateWindow("Tiko's Adventure Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	if (!win) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failure", SDL_GetError(), NULL);
		SDL_Quit();
		return false;
	}
	rend = SDL_CreateRenderer(win, -1, 0);
	if (!rend) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateRenderer failure", SDL_GetError(), NULL);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}
	puts("success");
	puts("initializing SDL_image...");
	if (!IMG_Init(IMG_INIT_PNG)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG_Init failure", IMG_GetError(), NULL);
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(rend);
		SDL_Quit();
		return false;
	}
	puts("success");
	puts("Initializing audio...");
	auto mix_flags = MIX_INIT_OGG | MIX_INIT_WAVPACK;

	if (!Mix_Init(mix_flags)) {
		assert(0 && "Mix_init screw up");
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Mix_OpenAudio failure", Mix_GetError(), NULL);
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(rend);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	SDL_Surface* icon = IMG_Load("sprites/icon.ico");
	if (icon) {
		SDL_SetWindowIcon(win, icon);
		SDL_FreeSurface(icon);
	}

	SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	puts("success");
	return true;
}
static void cleanup(void)
{
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(rend);
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
}

void game_loop(void);

int main(int argc, char** argv)
{
	if (!init())
		return -1;
	SDL_RenderSetLogicalSize(rend, SCREEN_WIDTH, SCREEN_HEIGHT);

	State *state = (State*) new Main_Menu_State();

	// Temporarily hardcoded. On implementation of more levels, they should store and play their own music.
	Mix_Music *music = g_resources.load_music("sounds/music/lvl1loop.ogg");
	assert(Mix_GetMusicLoopStartTime(music) > 0);

	constexpr float MUSIC_VOLUME = 0.8f;

	Mix_VolumeMusic((int)(128 * MUSIC_VOLUME));
	bool quit = false;
	while (!quit) {

		process_input();
		if (g_input.quit == true)
			quit = true;
		
		if (state->next_state != nullptr) {
			State *prev_state = state;
			state = state->next_state;
			delete prev_state;
			if (typeid(*state) == typeid(Main_Game_State))
				Mix_PlayMusic(music, -1);
			continue;
		}

		state->update();
		SDL_RenderClear(rend);
		state->render();
		SDL_RenderPresent(rend);
		SDL_Delay(1000 / FRAMERATE);
	}
	cleanup();
	return 0;
}