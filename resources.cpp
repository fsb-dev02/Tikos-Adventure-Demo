#include "resources.hpp"

Resources g_resources;
extern SDL_Renderer* rend;

Resources::Resources()
{
}

SDL_Texture* Resources::load_texture(const char *filename)
{
	const auto ret = textures.find(filename);
	if (ret == textures.end()) { // texture not found, isn't allocated yet
		SDL_Texture *tex = IMG_LoadTexture(rend, filename);
		if (!tex) {
			SDL_Log("WARNING: failed loading texture at path \"%s\"!\n", filename);
		}
		textures.insert({ filename, tex });
		return tex;
	}
	else {
		return ret->second;
	}
}

Mix_Chunk* Resources::load_sfx(const char* filename)
{
	const auto ret = sfx.find(filename);
	if (ret == sfx.end()) { // sound effect not found, isn't allocated yet
		Mix_Chunk* sound = Mix_LoadWAV(filename);
		if (!sound)
			SDL_Log("WARNING: failed loading WAV at path \"%s\"!\n", filename);
		sfx.insert({ filename, sound });
		return sound;
	}
	else {
		return ret->second;
	}
}

Mix_Music* Resources::load_music(const char* filename)
{
	const auto ret = music.find(filename);
	if (ret == music.end()) { // music track not found, isn't allocated yet
		Mix_Music* mus = Mix_LoadMUS(filename);
		if (!mus)
			SDL_Log("WARNING: failed loading music at path \"%s\"!\n", filename);
		music.insert({ filename, mus });
		return mus;
	}
	else {
		return ret->second;
	}
}

Resources::~Resources()
{
	// Clean up textures and sound

	for (auto &i : textures) {
		if (i.second != NULL)
			SDL_DestroyTexture(i.second);
	}
	for (auto &i : sfx) {
		if (i.second != NULL)
			Mix_FreeChunk(i.second);
	}
	for (auto &i : music) {
		if (i.second != NULL)
			Mix_FreeMusic(i.second);
	}
}