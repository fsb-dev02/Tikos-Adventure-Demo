#pragma once
#include <SDL_Mixer.h>
#include <vector>

/*
sound.hpp

Intended for easier handling of sound effects by the game's various objects.

*/

class Sfx;
typedef int Sound_ID;

class Sound_Manager {
	std::vector<Sfx> sounds;

public:
	Sound_ID load(const char *filename);
	void play(Sound_ID id);
};
extern Sound_Manager g_sound;


class Sfx {
	Mix_Chunk* audio;
	float volume = 1.0f;
	bool playing = false;
};

class Music {
	Mix_Music *audio;
	float volume = 1.0f;
	bool playing = false;
public:
	Music(const char* filename, int loop_point = 0);
	~Music();
	void play();
	void stop();
	inline void set_volume(float vol) { volume = vol; }
};

void play_sound(const char* filename, int channel = -1);
void play_sound(Mix_Chunk *sound, int channel = -1);