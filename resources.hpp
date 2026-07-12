#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <unordered_map>
#include <string>

/*
Resources

A class for easy loading of textures and audio files by whichever class needs them.
Handles its own allocation and cleanup, and performs checks to avoid allocating the same resource twice.

*/

class Resources {
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::unordered_map<std::string, Mix_Chunk*> sfx;
	std::unordered_map<std::string, Mix_Music*> music;
	//SDL_Texture* error_texture;

public:
	SDL_Texture *load_texture(const char * filename);
	Mix_Chunk* load_sfx(const char* filename);
	Mix_Music* load_music(const char* filename);
	Resources();
	~Resources();
};

extern Resources g_resources;