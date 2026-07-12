#pragma once
#include "entity.hpp"

/*
Spring

A spring object. Launches the player up when touched.

*/

class Spring :
    public Entity
{
    enum State {IDLE, } state;

public:
    Spring(SDL_FPoint pos);

    void collide_with_obj(const Entity& e);
};

