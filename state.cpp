#include <algorithm>
#include "sound.hpp"
#include "state.hpp"
#include <cassert>
#include "input.hpp"

extern SDL_Renderer* rend;
#define FADE_DURATION 800

Main_Game_State::Main_Game_State(const char* level_path, const char* bg_path):
	level_path(level_path), bg_path(bg_path)
{
	level = new Level(level_path);
	if (bg_path)
		level->load_bg(rend, bg_path);

	start_fadein(FADE_DURATION);
	start_time = SDL_GetTicks();
}

Main_Game_State::~Main_Game_State()
{
	if (level)
		delete level;
}

void Main_Game_State::update()
{

	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_ResetKeyboard();
		level->toggle_pause();
		if (level->is_paused())
			Mix_PauseMusic();
		else
			Mix_ResumeMusic();
	}
#ifdef _DEBUG
	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F]) {
		SDL_ResetKeyboard();
		goto finish_cheat;
	}
#endif

	level->update();
	if (level->is_finished() && !finish_handled) {
	finish_cheat:

		g_score.time = get_completion_time();
		finish_handled = true;
		Mix_FadeOutMusic(800);
		start_fadeout(FADE_DURATION, [this] {this->next_state = new Finish_State(); });
		return;
	}
	if (level->should_reset()) {
		level->toggle_reset();

		start_fadeout(FADE_DURATION, [this] {this->reset();});
	}
}

void Main_Game_State::render()
{
	level->render();

	// apply fade effect, if any

	uint8_t tint = 0xff * tint_float;
	SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, tint);
	SDL_RenderFillRect(rend, NULL);
	if (fading_in || fading_out) {
		tint_float += fade_per_tick;
		if (tint_float < 0.0f) { // fadein end
			tint_float = 0.0f;
			fade_per_tick = 0.0f;
			fading_in = false;
		}
		else if (tint_float > 1.0f) { // fadeout end
			tint_float = 1.0f;
			fade_per_tick = 0.0f;
			fading_out = false;
			fadeout_finished = true;
			fadeout_callback();
		}
	}
}

void Main_Game_State::start_fadein(int ms) {
	fading_in = true;
	tint_float = 1.0;
	fade_per_tick = -1 / (FRAMERATE * (ms / 1000.0));
}

void Main_Game_State::start_fadeout(int ms, std::function<void()> callback) {
	fadeout_finished = false;
	fading_out = true;
	tint_float = 0.0;
	fade_per_tick = 1 / (FRAMERATE * (ms / 1000.0));
	fadeout_callback = callback;
}

//Returns the level completion time in seconds.
int Main_Game_State::get_completion_time()
{
	return (SDL_GetTicks() - start_time) / 1000;
}

void Main_Game_State::reset() {
	puts("Level should be resetting now");
	next_state = new Main_Game_State(this->level_path, this->bg_path);
}

#define TITLE_PATH "sprites/title.png"
#define TITLE_W 192
#define TITLE_H 64

Main_Menu_State::Main_Menu_State()
{
	title = g_resources.load_texture(TITLE_PATH);
	assert(title);
	title_w = TITLE_W;
	title_h = TITLE_H;
}

// Most of this is provisional code, it should be edited to be more modular instead of direcly calling for the keyboard state.
void Main_Menu_State::update()
{
	int num_options = sizeof(options) / sizeof(options[0]);

	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN]) {
		SDL_ResetKeyboard();
		++cur_option;
		play_sound("sounds/coin_pickup.wav");
	}
	else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]) {
		SDL_ResetKeyboard();
		--cur_option;
		play_sound("sounds/coin_pickup.wav");
	}
	if (cur_option < 0)
		cur_option = 0;
	if (cur_option >= num_options)
		cur_option = num_options - 1;

	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RETURN])
		options[cur_option].action();
}

void Main_Menu_State::render()
{
	const SDL_Rect title_rect = { SCREEN_WIDTH / 2 - title_w / 2, 16, title_w, title_h };

	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0xff);
	SDL_RenderClear;

	SDL_RenderCopy(rend, title, NULL, &title_rect);
	int i = 0;
	for (auto option : options) {
		std::string cur_text = option.text;
		if (i == cur_option)
			cur_text = ">" + option.text;
		else
			cur_text = " " + option.text;
		text.render_text(cur_text, option.pos);
		++i;
	}
}

State* start_main_game() {
	g_score.deaths = 0;
	return (State*) new Main_Game_State("levels/rock_mnt.tmj", "sprites/bluesky.bmp");
}

const int Finish_State::scr_displays[4] = {
		FRAMERATE * 2,	// coins collected, 2s
		FRAMERATE * 2 * 3 / 4,	// time spent, 1.5s
		FRAMERATE,	// deaths, 1s
		FRAMERATE / 2// "try X next time", 0.5s
};

Finish_State::Finish_State()
{
	score_ticker = (int) (FRAMERATE * 2.5);
}

void Finish_State::update()
{
	if (score_ticker > 0)
		--score_ticker;

	if (score_ticker > 0 && score_ticker % (FRAMERATE / 2) == 0)
		play_sound("sounds/coin_pickup.wav");


	if (score_ticker == 0 && g_input.any_key)
		back_to_menu();
}

void Finish_State::render()
{
	const int font_h = text.get_font_char_h();
	const int line_spacing = 4;
	const int coins_ypos = -(SCREEN_HEIGHT / 2) + font_h * 2;
	const int time_ypos = coins_ypos + font_h + line_spacing;
	const int deaths_ypos = time_ypos + font_h + line_spacing;
	const int challenge_ypos = deaths_ypos + font_h + line_spacing;

	char coin_score[100];
	char time_score[100];
	char death_score[100];
	char challenge_line[100];

	sprintf(coin_score, "Coins collected:    %d", g_score.coins);
	sprintf(time_score, "Time spent:    %d'%d", g_score.time / 60, g_score.time % 60);
	sprintf(death_score, "Deaths:    %d", g_score.deaths);
	if (g_score.coins < 100)
		sprintf(challenge_line, "Try collecting 100 coins next time!");
	else if (g_score.time > CHALLENGE_TIME)
		sprintf(challenge_line, "Try finishing under %d'%d next time!", CHALLENGE_TIME / 60, CHALLENGE_TIME % 60);
	else
		//Probably never gonna be shown
		sprintf(challenge_line, "You have done the impossible. Well, congrats.");

	if (score_ticker < scr_displays[0])
		text.render_text(coin_score, { 0,  coins_ypos}, true, true);
	if (score_ticker < scr_displays[1])
		text.render_text(time_score, { 0, time_ypos }, true, true);
	if (score_ticker < scr_displays[2])
		text.render_text(death_score, { 0, deaths_ypos }, true, true);
	if (score_ticker < scr_displays[3])
		text.render_text(challenge_line, { 0, challenge_ypos }, true, true);
	if (score_ticker <= 0) {
		text.render_text("Thanks for playing! =D", { 0, 0 }, true, true);
		if ((SDL_GetTicks() >> 9) % 2) { // flashing text trick
			text.render_text("Press any key to continue", { 0, 12 }, true, true);
		}
	}
}

void Finish_State::back_to_menu()
{
	this->next_state = new Main_Menu_State();
}
