#pragma once
#include <cstdint>
#include <functional>

#include "level.hpp"
#include "hud.hpp"
#include "global_constants.hpp"
#include "score.hpp"

/*
State

A class representing a substate of the program's execution (think a "scene" in other engines) that can be updated and rendered independently.
If needed, a state may create another state and store it in the appropriate field so that the main loop may perform the switch. It is
the responsibility of the main loop to delete a previous state after it has switched to this new one.

*/

class State {
public:
	State* next_state = nullptr;
	virtual void update() = 0;
	virtual void render() = 0;
};

struct Menu_Option {
	std::string text;
	std::function<void()> action;
	SDL_Point pos;
};

//TEMPORARY HACK
State *start_main_game();

class Main_Menu_State : public State {
	Text_Renderer text;
	int cur_option = 0;
	SDL_Texture* title;
	int title_w;
	int title_h;

	const Menu_Option options[2] = {
	{"START GAME", [this] {g_score.reset_everything(); this->next_state = start_main_game(); }, {100, 96}},
	{"EXIT", [] {std::exit(EXIT_SUCCESS); }, {100, 112}}
	};
public:

	Main_Menu_State();
	void update() override;
	void render() override;
};

class Main_Game_State : public State {
	const char* level_path;
	const char* bg_path;
	Level *level;
	int start_time;
	
	std::function<void()> fadeout_callback;
	double tint_float = 0.0;
	double fade_per_tick; // for transitions
	bool fading_out = false;
	bool fading_in = false;
	bool fadeout_finished = false;

	bool finish_handled = false;

public:

	Main_Game_State(const char* level_path, const char *bg_path = NULL);
	~Main_Game_State();
	void update() override;
	void render() override;

	void reset();
	void start_fadein(int ms);
	void start_fadeout(int ms, std::function<void()> callback = []{});
	int get_completion_time();
};

class Finish_State : public State {
	Text_Renderer text;
	int score_ticker;
	// Time at which scores are shown during the tickdown.
	static const int scr_displays[4];

public:
	Finish_State();
	void update() override;
	void render() override;

	void back_to_menu();
};