#pragma once

/*
Score

Handles global score counting by means of a singleton object.
*/

struct Score
{
	int coins = 0;
	int time = 0;
	int deaths = 0;

	void reset_coins();
	void reset_time();
	void reset_everything();
};

extern Score g_score;