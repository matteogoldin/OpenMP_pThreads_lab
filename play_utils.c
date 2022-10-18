#include "play_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROB 97

pball_t ball;

pball_t *getBall()
{
	return &ball;
}

void play_init()
{
	ball.x = MAP_WIDTH - BALL_WIDTH;
	ball.y = MAP_HEIGHT - BALL_HEIGHT;
	ball.dy = 2;
	ball.dx = 2;
}

uint32_t getBallXPOS()
{
	return ball.x + MAP_XOFFSET;
}

uint32_t getBallYPOS()
{
	return ball.y + MAP_YOFFSET;
}

#define BOUND  30
void play_move_ball()
{

	ball.x += ball.dx;
	ball.y += ball.dy;

	int r = rand() % 100;
	
	if (ball.dx > BOUND ||  ball.dx < -BOUND || ball.dy > BOUND || ball.dy < -BOUND){
		ball.dx=1;
		ball.dy=1;
	}
	if (r > PROB)
	{
		if (r > (PROB + (100 - PROB) / 2))
		{
			ball.dx = ball.dx * (r * 2 / 100.);
			ball.dy = -ball.dy * (r * 2 / 100.);
		}
		else
		{
			ball.dx = ball.dx * (r * 2 / 100.);
			ball.dy = ball.dy * (r * 2 / 100.);
		}
	}

	if (ball.x < 0)
	{
		ball.dx = -ball.dx;
	}

	if (ball.x + BALL_WIDTH > MAP_WIDTH)
	{
		ball.dx = -ball.dx;
	}

	if (ball.y < 0)
	{
		ball.dy = -ball.dy;
	}
	if (ball.y + BALL_HEIGHT > MAP_HEIGHT)
	{
		ball.dy = -ball.dy;
	}

	if (ball.y > MAP_HEIGHT)
		ball.y = MAP_HEIGHT;
	if (ball.y < 0)
		ball.y = 0;

	if (ball.x > MAP_WIDTH)
		ball.x = MAP_HEIGHT;
	if (ball.x < 0)
		ball.x = 0;
}
