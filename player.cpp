#include "helpers.hpp"
#include "resources.hpp"
#include "input.hpp"
#include "level.hpp"
#include "player.hpp"
#include "entity_factory.hpp"

#include "global_constants.hpp"
#include "score.hpp"

constexpr SDL_FPoint PLAYER_SIZE = { 14, 14 };

Player::Player(SDL_FPoint pos)
	:Entity(NULL, pos, PLAYER_SIZE)
{
	const char* ANIM_PATH = "sprites/ball/beach_ball.png";
	SDL_Texture* anim_tex = g_resources.load_texture(ANIM_PATH);

	type = PLAYER;
	state = ACTIVE;
	anim = new Animation(anim_tex, 24, 24, 12, 4 * COMPAT_TICK_MULTIPLIER, { 5, 10 });
}

Player::~Player()
{
	delete anim;
}

void Player::update(Level *level)
{
	const float gravity		= (kgravity) * PHYS_SCALE;
	const float walk_accel	= (0.2f) * ACCEL_SCALE;
	const float deccel		= (0.4f) * PHYS_SCALE;
	const float jump_per_tick = (1.4f) * PHYS_SCALE;

	const float max_x_vel = 3.0f;
	const float max_y_vel = 10.0f;
	const float maxjump = 4.5f;
	const float bounce		= 7.5f;
	const float stop_x_vel = cur_platform ? cur_platform->vel.x : 0.0f;
	const float stop_y_vel = cur_platform ? cur_platform->vel.y : 0.0f;

	const int max_jump_ticks = 8 * COMPAT_TICK_MULTIPLIER; // originally 8 ticks at 60fps

	bool was_above = false; // Determines whether we can land on a one-way tile or not

	const int tw = COLLISION_TW;
	const int th = COLLISION_TH;

	prev_pos = pos;

	switch (state) {
	case ACTIVE:
		// Die on bottomless pit
		if (pos.y > level->get_px_h()) {
			this->kill();
			break;
		}

		if (cur_platform)
			pos += (cur_platform->vel * PHYS_SCALE);

		if (!on_ground && !cur_platform) {
			vel.y += gravity;
		}
		else {
			vel.y = 0;
		}

		if (g_input.right == true) {
			vel.x += walk_accel;
			if (vel.x > max_x_vel)
				vel.x = max_x_vel;
		}
		else if (g_input.left == true) {
			vel.x -= walk_accel;
			if (vel.x < -max_x_vel)
				vel.x = -max_x_vel;
		}
		else if (on_ground || cur_platform) {
			if (vel.x > 0) {
				vel.x -= deccel;
				if (vel.x < 0)
					vel.x = 0;
			}
			else if (vel.x < 0) {
				vel.x += deccel;
				if (vel.x > 0)
					vel.x = 0;
			}
		}

		if (g_input.jump == true && (on_ground || cur_platform)) {
			jumping = true;
			play_sound("sounds/jump.wav", CHANNEL_JUMP);
			on_ground = false;
			
		}
		if (jumping) {
			if (g_input.jump == true && ++jump_ticker < max_jump_ticks) {
				vel.y = vel.y < -maxjump ? -maxjump : vel.y - jump_per_tick;
			}
			else {
				jump_ticker = 0;
				jumping = false;
			}
		}
		
		was_above = test_bottom_collision(*level, *this) != Collision_Map::ONE_WAY;
		//if (cur_platform) {
		//	vel += cur_platform->vel;
		//}

		pos.x += vel.x * PHYS_SCALE;
		if (cur_platform && !test_x_overlap(*this, *cur_platform))
			cur_platform = NULL;
		if (vel.x > 0) {
			if (test_right_collision(*level, *this)) {
				pos.x = SNAP_TO_GRID(COLLISION_TW, right()) - size.x;
				vel.x = 0;
			}
		}
		else if (vel.x < 0) {
			if (test_left_collision(*level, *this)) {
				pos.x = SNAP_TO_GRID(COLLISION_TW, left()) + COLLISION_TW;
				vel.x = 0;
			}
		}
		
		if (vel.y > max_y_vel)
			vel.y = max_y_vel;
		pos.y += vel.y * PHYS_SCALE;
		if (cur_platform && bottom() < cur_platform->top() - 1)
			cur_platform = NULL;

		if (vel.y >= 0) /* not > 0, as we account for gravity */ {
			switch (test_bottom_collision(*level, *this)) {
			case Collision_Map::NONE:
				on_ground = false;
				break;
			case Collision_Map::ONE_WAY:
				if (!was_above)
					break;
				// else fall through
			case Collision_Map::SOLID:
			default: // default here is to account for cases where the hotspots differ
				on_ground = true;
				pos.y = SNAP_TO_GRID(COLLISION_TH, bottom()) - size.y;
				if (g_input.jump) {
					this->bounce(bounce);
					play_sound("sounds/jump.wav", CHANNEL_JUMP);
				}
			}
		}
		else {
			if (test_top_collision(*level, *this)) {
				pos.y = SNAP_TO_GRID(COLLISION_TH, top()) + th;
				vel.y = 0;
			}
		}
		for (const auto& platform : level->platforms) {
			if (platform.get() == cur_platform)
				continue;
			if (test_platform_collision(*platform)) {
				puts("collided with platform");
				on_ground = true;
				pos.y = platform->pos.y - size.y;
				if (g_input.jump) {
					this->bounce(bounce);
					play_sound("sounds/jump.wav", CHANNEL_JUMP);
				}
				else {
					cur_platform = platform.get();
				}
			}
		}
		anim->update();
		break;
	case DYING:
		vel.y += gravity;
		pos.y += vel.y * PHYS_SCALE;
		if (pos.y > level->get_px_h()) {
			state = DEAD;
		}
		break;
	case DEAD:
		level->restart();
		state = State::DELETED;
		break;
	case DELETED:
		;
	}
}

void Player::collide_with_obj(const Entity& e)
{
	if (is_dead())
		return;
	switch (e.get_type()) {
	case Entity::BALLOON:
		this->kill();
		break;
	case Entity::COIN:
		if (reinterpret_cast<const Coin*>(&e)->get_state() == Coin::ACTIVE) {
			coins++;
		}
		break;
	case Entity::SPRING:
		this->bounce(10.0f);
		play_sound("sounds/spring.wav", CHANNEL_SPRING);
		break;
	case Entity::BOMB:
		this->kill();
		break;
	case Entity::EXPLOSION:
		this->kill();
		break;
	}
}

bool Player::test_platform_collision(const Entity& platform)
{
	// Note: this assumes the platform's y origin is exactly at the top.
	return /*vel.y - platform.vel.y >= 0 &&*/
		prev_pos.y + size.y < platform.prev_pos.y &&
		bottom() > platform.pos.y &&
		test_x_overlap(*this, platform);
}

void Player::bounce(float target_vel)
{
	vel.y = -target_vel;
	on_ground = false;
}

void Player::kill()
{
	if (is_dead()) return;

	g_score.deaths++;
	Mix_HaltMusic();
	play_sound("sounds/death.wav");
	state = DYING;
	vel.y = -10.0f;
}
