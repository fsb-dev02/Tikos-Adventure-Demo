#include "score.hpp"

void Score::reset_coins()
{
	coins = 0;
}

void Score::reset_time()
{
	time = 0;
}

void Score::reset_everything() {
	coins = 0;
	time = 0;
	deaths = 0;
}

Score g_score;