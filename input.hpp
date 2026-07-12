#pragma once
#include <SDL.h>

/*
	Input information to be read in-game by controllable objects, without dependance on SDL.
*/

void process_input(void);

struct input {
	bool quit = false;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool jump = false;
	bool any_key = false;
};

extern struct input g_input;