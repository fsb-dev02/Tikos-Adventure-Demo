#pragma once
#include <string>
#include <array>
#include <SDL.h>
#include <functional>
#include "resources.hpp"

/*
hud.hpp

Objects designed to be displayed over the screen, in ignorance of level physics, go here.
Also used for menu screens.

*/


// For a future implementation of in-game score counters.
class GUI_Element {
	SDL_Point pos;
	SDL_Point size;
	std::function<void()> on_select;
};

class Text_Renderer {
	SDL_Texture* font_tex;
	SDL_Point tex_size;
	SDL_Rect font_lookup[128];
	//SDL_Rect retrieve_char(char c);
	SDL_Point font_size;

public:
	Text_Renderer();
	void render_text(const char* s, SDL_Point pos = { 0, 0 }, bool center_horizontally = false, bool center_vertically = false);
	void render_text(std::string s, SDL_Point pos = { 0,0 });
	inline int get_font_char_w() { return font_size.x; }
	inline int get_font_char_h() { return font_size.y; }
};