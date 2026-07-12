#include <iostream>
#include <vector>
#include <map>
#include <SDL.h>

SDL_Texture* t_img;
std::vector <int> tilemap;
std::map<int, SDL_Rect> lookup;


void load_level(void);