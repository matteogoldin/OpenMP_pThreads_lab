#ifndef UTILS_H
#define UTILS_H

#define BALL_WIDTH 15
#define BALL_HEIGHT 15

#define RACKET_WIDTH 50
#define RACKET_HEIGHT 10

#define MAP_WIDTH 600
#define MAP_HEIGHT 600
#define MAP_XOFFSET 50
#define MAP_YOFFSET 50

#include <stdint.h>

typedef struct pball_s
{
    int id;
    int x, y;
    int dx, dy;
    double speed; // may be represent it as time
} pball_t;

typedef struct racket_s
{
    int x, y;
} racket_t;

pball_t *getBall();
racket_t *getRacket();
void play_init();


void play_move_ball();

uint32_t getBallXPOS();
uint32_t getBallYPOS();

#endif