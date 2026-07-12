#pragma once
#include <SDL.h>
#include <SDL_image.h>

/*
Animation
Stores, updates and renders an animation generated from an horizontal spritesheet, loaded in the format of an SDL_Texture.
w and h refer to the width and height of the frames. You should make yourself certain that both the w parameter and num_frames actually match with
the width of the texture when using this class.

*/

class Animation {
public:
	Animation(SDL_Texture *tex, int w, int h, int num_frames, int ticks_per_frame, SDL_Point offset = { 0, 0 }, SDL_Point texture_offset = {0, 0});
	~Animation();
	SDL_Texture* texture;
	SDL_Point texture_offset = { 0, 0 };
	SDL_Rect cur_rect;
	int cur_frame;

	void render(SDL_Renderer* rend, const SDL_Rect* dst = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void update(void);
	void set_frame(int n);
	void set_loop(bool status);
	bool is_over();

	const int frame_w;
	const int frame_h;
	const SDL_Point offset;

private:
	const int num_frames;
	const int ticks_per_frame;
	int frame_ticker;
	bool loop = true;
	bool stop = false;

};