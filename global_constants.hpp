#pragma once

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 180

#define FRAMERATE 240

// timestep stuff
#define PHYS_SCALE (60.0f / FRAMERATE)
#define ACCEL_SCALE 0.5f
#define COMPAT_TICK_MULTIPLIER (FRAMERATE / 60) // For converting animation speeds and other things originally thought for 60fps

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define COLLISION_TW 8
#define COLLISION_TH 8

// Certain sound effects in the game might end up overlapping multiple times due to being played too often, resulting in excessive loudness and distortion.
// We solve this problem by allocating them to dedicated channels. 
#define CHANNEL_JUMP 1
#define CHANNEL_COIN 2
#define CHANNEL_SPRING 3

// For "try beating it in under X time" on the scoreboard
#define CHALLENGE_TIME 50