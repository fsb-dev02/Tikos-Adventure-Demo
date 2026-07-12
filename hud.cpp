#include "hud.hpp"
#include "global_constants.hpp"
#include <cassert>

// TODO: refactpr this and leave the character lookup tables, font and other things to the resource allocator.
// TODO: add support for loading whatever custom font instead of having a hardcoded one.

extern SDL_Renderer* rend;

#define CHAR_W 7
#define CHAR_H 9

#define FONT_PATH "fonts/main_menu.png" // For use only until more fonts are added
#define FONT_CHARSET " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define FONT_TEX_SIZE {112,54}
#define FONT_WIDTH 16
#define FONT_HEIGHT 6

Text_Renderer::Text_Renderer()
{
	const char* tex_path = FONT_PATH;

	font_tex = g_resources.load_texture(tex_path);
	assert(font_tex && "ERROR LOADING FONT TEXTURE");
	tex_size = FONT_TEX_SIZE;
	int font_w = FONT_WIDTH; // measured in character count
	int font_h = FONT_HEIGHT;

	//Generate a lookup table for retrieving characters
	const char* charset = FONT_CHARSET;

	for (int i = 0; charset[i] != '\0'; ++i) {
		SDL_Rect cur_rect = {CHAR_W * (i % font_w), CHAR_H * (i / font_w), CHAR_W, CHAR_H};
		font_lookup[charset[i]] = cur_rect;
	}

	font_size = { CHAR_W, CHAR_H };
}

void Text_Renderer::render_text(const char *s, SDL_Point pos, bool center_h, bool center_v)
{
	constexpr int font_spacing = CHAR_W;
	const int len = strlen(s);
	const int x_offset = center_h ? SCREEN_WIDTH / 2 - (len * CHAR_W) / 2 : 0;
	const int y_offset = center_v ? SCREEN_HEIGHT / 2 - CHAR_H / 2 : 0;

	SDL_Rect cur_rect = { pos.x + x_offset, pos.y + y_offset, CHAR_W, CHAR_H };
	while (*s) {
		SDL_RenderCopy(rend, font_tex, &(font_lookup[*s]), &cur_rect);
		cur_rect.x += font_spacing;
		++s;
	}
}

void Text_Renderer::render_text(std::string s, SDL_Point pos)
{
	constexpr int font_spacing = 8;

	SDL_Rect cur_rect = { pos.x, pos.y, CHAR_W, CHAR_H };
	for (char c : s) {
		SDL_RenderCopy(rend, font_tex, &(font_lookup[c]), &cur_rect);
		cur_rect.x += font_spacing;
	}
}


