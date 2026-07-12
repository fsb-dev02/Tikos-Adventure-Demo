#include <cassert>
#include "platform.hpp"
#include "helpers.hpp"
#include "resources.hpp"
#include "global_constants.hpp"

Platform::Platform(const Platform_Definition& def, float speed)
    :Entity(anim, pos, { (float) def.w ,(float) def.h }), speed(speed)
{
    this->type = Entity::Type::PLATFORM;

    SDL_Texture* tex = g_resources.load_texture(def.texture_path);
    anim = new Animation(tex, def.w, def.h, 1, 0, { 0,0 }, { def.texture_offset_x, def.texture_offset_y });

}

Platform::~Platform()
{
    if (anim)
        delete anim;
}

void Platform::update(Level *level)
{
    // Move towards the next target. If it is reached, move onto next target or switch directions.
    move_towards(speed, targets[cur_tg]);
    if (at_target(targets[cur_tg])) {
        pos = targets[cur_tg];
        assert(dir == 1 || dir == -1);
        cur_tg += dir;
        if (!(cur_tg >= 0 && cur_tg < targets.size())) {
            if (loop) {
                cur_tg = 0;
            }
            else {
                dir = dir * -1;
                cur_tg += dir * 2; // Undoing the "out of bounds" step.
            }
        }
    }
}

inline void Platform::move_towards(float speed, SDL_FPoint dst)
{
    SDL_FPoint diff = dst - pos;
    float hypot = sqrt(diff.x * diff.x + diff.y * diff.y);
    if (hypot > 0.0f) {
        SDL_FPoint dir = diff / hypot;
        vel = dir * speed;
    }
    // in case of hypot == 0, intentionally keep previous velocity, so that overshoot is caught by at_target().
    prev_pos = pos;
    pos += vel * PHYS_SCALE;
}

inline bool Platform::at_target(SDL_FPoint target) const
{
    return  (vel.x > 0.0f && pos.x > target.x) ||
            (vel.x < 0.0f && pos.x < target.x) ||
            (vel.y > 0.0f && pos.y > target.y) ||
            (vel.y < 0.0f && pos.y < target.y);
}

void Platform::add_target(float x, float y)
{
    targets.push_back({ x, y });
}

void Platform::add_target(SDL_FPoint t)
{
    targets.push_back(t);
}

void Platform::start()
{
    pos = targets[0];
    cur_tg = 1;
}

static const Platform_Definition g_platform_defs[PLATFORM_TYPE_COUNT] = {
    {32, 8, "tilesets/summer_.png", 48, 256}, // GRASS_2X1_THIN
    {48, 8, "tilesets/summer_.png", 0, 256} // GRASS_3X1_THIN
};