#include <cassert>
#include "resources.hpp"
#include "animation.hpp"

extern SDL_Renderer* rend;

Animation::Animation(SDL_Texture *tex, int w, int h, int num_frames, int ticks_per_frame,
	SDL_Point offset, SDL_Point texture_offset)
	:texture(tex), frame_w(w), frame_h(h), num_frames(num_frames),
	ticks_per_frame(ticks_per_frame), offset(offset), texture_offset(texture_offset)
{
	frame_ticker = ticks_per_frame;
	cur_frame = 0;
	cur_rect.x = texture_offset.x;
	cur_rect.y = texture_offset.y;
	cur_rect.w = frame_w;
	cur_rect.h = frame_h;
}

Animation::~Animation()
{
}

void Animation::render(SDL_Renderer* rend, const SDL_Rect *dst,
	SDL_RendererFlip flip)
{
	if (!dst)
		SDL_RenderCopy(rend, texture, &cur_rect, NULL);
	else {
		SDL_Rect new_dst = {dst->x - offset.x, dst->y - offset.y, frame_w, frame_h};
		SDL_RenderCopyEx(rend, texture, &cur_rect, &new_dst,0.0f, NULL, flip);
	}
}

void Animation::update(void)
{
	if (stop || ticks_per_frame == 0)
		return;
	if (--frame_ticker <= 0) {
		frame_ticker = ticks_per_frame;
		cur_rect.x += frame_w;
		++cur_frame;
		if (cur_frame >= num_frames) {
			if (loop) {
				cur_rect.x = texture_offset.x;
				cur_frame = 0;
			}
			else {
				cur_frame = num_frames - 1;
				cur_rect.x = cur_frame * frame_w + texture_offset.x;
				stop = true;
			}
		}
	}
}

/*  Changes the current frame of animation and resets the ticker.
	All indexing starts with 0.
*/
void Animation::set_frame(int n)
{
	cur_frame = n;
	cur_rect.x = n * frame_w + texture_offset.x;
	frame_ticker = ticks_per_frame;
}

void Animation::set_loop(bool status)
{
	if (loop == status)
		return;
	if (status == true)
		set_frame(0);
	loop = status;

}

// Checks if the animation has ended. Only ever returns "true" if loop = false.
bool Animation::is_over()
{
	if (loop)
		return false;
	return stop;
}
