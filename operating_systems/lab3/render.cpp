#include "render.h"
#include <math.h> 
#include <syslog.h>
#include <string>
#include <cstdlib>

int width = 800; 
int height = 600; 
const int interval = 1000 / 60; 

static const int racket_width = 10;
static const int racket_height = 80;
static const int ball_size = 8;

static float racket_left_x = 10.0f; 
static float racket_left_y = 50.0f; 

static float racket_right_x = width - racket_width - 10; 
static float racket_right_y = 50 + (int)(rand() % 200);

static float ball_pos_x = width / 2 + (int)(rand() % 100) - 100; 
static float ball_pos_y = height / 2 + (int)(rand() % 100) - 100;

void set_racket_left(float x, float y)
{
	racket_left_x = x;
	racket_left_y = y;
}

void set_racket_right(float x, float y)
{
	racket_right_x = x;
	racket_right_y = y;
}

void set_ball(float x, float y)
{
	ball_pos_x = x;
	ball_pos_y = y;
}

void enable2D ( int width, int height ) 
{ 
	glViewport ( 0 , 0 , width, height ); 
	glMatrixMode ( GL_PROJECTION ); 
	glLoadIdentity ( ); 
	glOrtho ( 0.0f , width, 0.0f , height, 0.0f , 1.0f ); 
	glMatrixMode ( GL_MODELVIEW ); 
	glLoadIdentity ( ); 
}

void drawRect ( float x, float y, float width, float height ) 
{ 
	glBegin ( GL_QUADS ); 
	glVertex2f ( x, y ); 
	glVertex2f ( x + width, y ); 
	glVertex2f ( x + width, y + height ); 
	glVertex2f ( x, y + height ); 
	glEnd ( ); 
}

void draw ( ) 
{ 	
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glLoadIdentity ( ); 

	drawRect ( racket_left_x, racket_left_y, racket_width, racket_height ); 
	drawRect ( racket_right_x, racket_right_y, racket_width, racket_height ); 
	drawRect ( ball_pos_x - ball_size / 2 , ball_pos_y - ball_size / 2 , ball_size, ball_size );

	glutSwapBuffers ( );  
} 

void update ( int value ) 
{ 
	glutTimerFunc ( interval, update, 0 );

	glutPostRedisplay ( ); 
}

void render_init(int argc, char ** argv, int w, int h)
{
	width = w;
	height = h;
	
	glutInit ( & argc, argv ); 
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ); 
	glutInitWindowSize ( width, height ); 
	glutCreateWindow ( "Pong" ); 

	glutDisplayFunc ( draw ); 
	glutTimerFunc ( interval, update, 0 ); 
	
	enable2D ( width, height ); 
	glColor3f ( 1.0f , 1.0f , 1.0f ); 
	syslog(LOG_INFO, "Render init successful");
	glutMainLoop();
}
