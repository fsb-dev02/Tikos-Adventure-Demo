#include "input.hpp"
struct input g_input;

void process_input(void)
{

	g_input.any_key = false;
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			g_input.quit = true;
			break;
		case SDL_KEYDOWN:
			g_input.any_key = true;
		}
	}

	// Typing all this should certainly pay off... if i ever implement customizable controls.
	const SDL_Scancode jump = SDL_SCANCODE_SPACE;
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	if (kb[SDL_SCANCODE_UP])
		g_input.up = true;
	else
		g_input.up = false;

	if (kb[SDL_SCANCODE_DOWN])
		g_input.down = true;
	else
		g_input.down = false;

	if (kb[SDL_SCANCODE_LEFT])
		g_input.left = true;
	else
		g_input.left = false;

	if (kb[SDL_SCANCODE_RIGHT])
		g_input.right = true;
	else
		g_input.right = false;

	if (kb[jump])
		g_input.jump = true;
	else
		g_input.jump = false;

}