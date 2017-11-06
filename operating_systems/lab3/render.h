#ifndef RENDER_H_INCLUDED__
#define RENDER_H_INCLUDED__
#include <GL/glut.h>
#include <stdlib.h>

void render_init(int argc, char ** argv, int w, int h);
void set_racket_left(float x, float y);
void set_racket_right(float x, float y);
void set_ball(float x, float y);
#endif
