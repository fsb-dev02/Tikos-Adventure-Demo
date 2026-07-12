#pragma once
#include "entity.hpp"

/*
Explosion

Spawned by Bomb after the detonation animation finishes. Kills the player on contact, and despawns itself when its animation is over.

*/

class Explosion : public Entity
{
public:
    Explosion(SDL_FPoint pos);
    ~Explosion();
    void update(Level *level) override;
};

