#include "bomb.hpp"
#include "player.hpp"
#include "helpers.hpp"
#include "resources.hpp"
#include "level.hpp"
#include "global_constants.hpp"

constexpr SDL_FPoint BOMB_SIZE = {11.0f, 17.0f};
extern Resources g_resources;

Bomb::Bomb(SDL_FPoint pos = { 0.0f, 0.0f })
	:Entity(anim, pos, BOMB_SIZE)
{
	type = Entity::Type::BOMB;

	constexpr int frame_w = 24;
	constexpr int frame_h = 24;
	constexpr int num_frames = 4;
	constexpr int frame_duration = 10 * COMPAT_TICK_MULTIPLIER;

	SDL_Texture* anim_tex = g_resources.load_texture("sprites/bomb/Bomb.png");


	constexpr SDL_Point idle_offset = { 7, 7 };
	anim_idle = new Animation(anim_tex, frame_w, frame_h,
		num_frames, frame_duration,
		idle_offset);

	constexpr SDL_Point aggro_offset = { 7, 7 };
	constexpr SDL_Point aggro_texture_offset = { 0, 168 };
	anim_aggro = new Animation(anim_tex, frame_w, frame_h,
		num_frames, frame_duration,
		aggro_offset, aggro_texture_offset);

	constexpr SDL_Point expl_offset = { 7, 7 };
	constexpr SDL_Point expl_texture_offset = { 0, 216 };
	constexpr int expl_duration = 6 * COMPAT_TICK_MULTIPLIER;
	anim_explosion = new Animation(anim_tex, frame_w, frame_h, num_frames, expl_duration,
		expl_offset, expl_texture_offset);
	anim_explosion->set_loop(false);

	sfx_blast = g_resources.load_sfx("sounds/explosion04.wav");

	state = IDLE;
	anim = anim_idle;
	direction = RIGHT;
}

Bomb::~Bomb()
{
	if (anim_idle)
		delete anim_idle;
	if (anim_aggro)
		delete anim_aggro;
	if (anim_explosion)
		delete anim_explosion;
}

void Bomb::update(Level *level)
{
	constexpr SDL_Point SCREEN_SIZE = { 320, 180 };

	constexpr float gravity = kgravity * PHYS_SCALE;
	constexpr float xaccel = 0.1f * ACCEL_SCALE;

	constexpr float aggro_distance = 80.0f;
	constexpr float max_x_speed = 2.0f;
	constexpr float jump_power = 6.0f;
	constexpr float attack_range = (jump_power / kgravity) * 2 * max_x_speed;

	const Player *player = level->get_player();


	bool visible = on_screen({ (int)pos.x, (int)pos.y, (int)size.x, (int)size.y },
		level->camera, SCREEN_SIZE);

	switch (state) {
	case IDLE:
		if (anim != anim_idle)
			anim = anim_idle;
		if (visible && distance(pos, player->pos) < aggro_distance)
			aggro();
		break;
	case AGGRO:
		if (!visible) {
			state = IDLE;
			break;
		}

		if (player->pos.x < pos.x)
			direction = LEFT;
		if (player->pos.x > pos.x)
			direction = RIGHT;

		vel.x += xaccel * direction;
		if (vel.x > max_x_speed)
			vel.x = max_x_speed;
		else if (vel.x < -max_x_speed)
			vel.x = -max_x_speed;

		// jump at just the right moment
		if (on_ground && fabs(vel.x) == max_x_speed && distance(pos, player->pos) < attack_range) {
			this->jump(jump_power);
		}

		// if falling towards the player, go KABOOM!
		if (!on_ground && fabs(pos.x - player->pos.x) < attack_range / 2) {
			detonate();
		}
		break;
	case EXPLODING:
		if (on_ground)
			vel.x /= 2; // Decceleration.
		if (anim->is_over()) {
			level->spawn_entity(Type::EXPLOSION, pos.x, pos.y);
			play_sound(sfx_blast, -1);
			this->despawn();
			return;
		}
	default:
		;
	}
	if (!on_ground)
		vel.y += gravity;
	{
		const int tw = COLLISION_TW;
		const int th = COLLISION_TH;

		pos.x += vel.x * PHYS_SCALE;
		if (vel.x > 0) {
			if (test_right_collision(*level, *this)) {
				pos.x = SNAP_TO_GRID(tw, pos.x) + tw - size.x;
				vel.x = 0;
				if (state == AGGRO)
					jump(jump_power);
			}
		}
		else if (vel.x < 0) {
			if (test_left_collision(*level, *this)) {
				pos.x = SNAP_TO_GRID(tw, pos.x) + tw;
				vel.x = 0;
				if (state == AGGRO)
					jump(jump_power);
			}
		}

		// todo: add proper one-way platform handling

		pos.y += vel.y * PHYS_SCALE;
		if (vel.y >= 0) /* not > 0, as we account for gravity */ {
			if (test_bottom_collision(*level, *this)) {
				pos.y = SNAP_TO_GRID(th, pos.y + size.y) - size.y;
				vel.y = 0;
				on_ground = true;
			}
			else on_ground = false;
		}
		else {
			if (test_top_collision(*level, *this)) {
				pos.y = SNAP_TO_GRID(th, pos.y) + th;
				vel.y = 0;
			}
		}
	}

	anim->update();
}

void Bomb::collide_with_obj(const Entity& e)
{
	switch (e.get_type()) {
	case Entity::Type::PLAYER:
	case Entity::Type::BALLOON:
	case Entity::Type::EXPLOSION:
		detonate();
	default:
		;
	}
}

void Bomb::detonate()
{
	//play_sound(sfx_blast);
	state = EXPLODING;
	anim = anim_explosion;
}

void Bomb::aggro()
{
	state = AGGRO;
	anim = anim_aggro;
}

inline void Bomb::jump(float power)
{
	const int jump_delay = 5;
	static int jump_ticker = jump_delay;

	if (!jump_ticker--) {
		vel.y = -power;
		on_ground = false;
		jump_ticker = jump_delay;
	}
}

void Bomb::render(SDL_Renderer *rend, SDL_Point offset)
{
	if (!anim)
		return;
	const SDL_Rect dst = { (int)pos.x - offset.x, (int)pos.y - offset.y, size.x, size.y };
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (direction == LEFT)
		flip = SDL_FLIP_HORIZONTAL;
	if (direction == RIGHT)
		flip = SDL_FLIP_NONE;
	anim->render(rend, &dst, flip);
}
