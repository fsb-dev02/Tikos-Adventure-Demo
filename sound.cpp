#include "sound.hpp"
#include "resources.hpp"

void play_sound(const char* filename, int channel)
{
	Mix_Chunk* sound = g_resources.load_sfx(filename);
	Mix_PlayChannel(channel, sound, 0);
}

void play_sound(Mix_Chunk* sound, int channel)
{
	Mix_PlayChannel(channel, sound, 0);
}



Music::Music(const char* filename, int loop_point)
{
	audio = g_resources.load_music(filename);
}

void Music::play()
{
	if (Mix_PlayingMusic())
		Mix_HaltMusic();
	Mix_PlayMusic(audio, 999);
	playing = true;
}

void Music::stop()
{
	if (this->playing) {
		Mix_HaltMusic();
		playing = false;
	}
}

Sound_ID Sound_Manager::load(const char* filename)
{
	return 0;
}
