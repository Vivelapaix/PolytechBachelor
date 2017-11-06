#include <math.h> 
#include "host.h"
#include "client.h"
#include "conn.h"
#include <syslog.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <memory>
#include <time.h>

static int width, height;

static float racket_right_x;
static float racket_right_y; 

static float racket_left_x; 
static float racket_left_y; 

static float ball_pos_x; 
static float ball_pos_y; 
static std::unique_ptr<conn_t> conn;
float ball_dir_x; 
float ball_dir_y;

void vec2_norm ( float & x, float & y ) 
{ 
	float length = sqrt ( ( x * x ) + ( y * y ) ); 
	if ( length != 0.0f ) 
	{ 
		length = 1.0f / length; 
		x *= length; 
		y *= length; 
	} 
}

void update_racket_right(float ball_position_y)
{
	if(racket_right_y + racket_height / 2 > ball_position_y + 0.5)
		racket_right_y -= racket_speed;
	if(racket_right_y + racket_height / 2 < ball_position_y - 0.5)
		racket_right_y += racket_speed;
		
	if(racket_right_y < 0.0f)
		racket_right_y = 0.0f;
	if(racket_right_y > height - racket_height)
		racket_right_y = height - racket_height;
}

void update_ball ( ) 
{ 
 
	ball_pos_x += ball_dir_x * ball_speed; 
	ball_pos_y += ball_dir_y * ball_speed; 

	if (ball_pos_x < racket_left_x + racket_width && 
		ball_pos_x > racket_left_x && 
		ball_pos_y < racket_left_y + racket_height && 
		ball_pos_y > racket_left_y ) 
	{ 
		float t = ( ( ball_pos_y - racket_left_y ) / racket_height ) - 0.5f + (rand() % 200 - 100) / 100.f; 
		ball_dir_x = fabs ( ball_dir_x );
		ball_dir_y = t; 
	} 

	if (ball_pos_x > racket_right_x && 
		ball_pos_x < racket_right_x + racket_width && 
		ball_pos_y < racket_right_y + racket_height && 
		ball_pos_y > racket_right_y ) 
	{ 
		float t = ( ( ball_pos_y - racket_right_y ) / racket_height ) - 0.5f + (rand() % 200 - 100) / 100.f; 
		ball_dir_x = - fabs ( ball_dir_x );
		ball_dir_y = t; 
	} 

	if (ball_pos_x < 0) 
	{ 
		ball_pos_x = width / 2; 
		ball_pos_y = height / 2; 
		ball_dir_x = fabs ( ball_dir_x );
		ball_dir_y = 0; 
	} 

	if (ball_pos_x > width) 
	{ 
		ball_pos_x = width / 2; 
		ball_pos_y = height / 2; 
		ball_dir_x = - fabs ( ball_dir_x );
		ball_dir_y = 0; 
	} 

	if ( ball_pos_y > height ) 
	{ 
		ball_dir_y = - fabs ( ball_dir_y ); 
	} 

	if ( ball_pos_y < 0 ) 
	{ 
		ball_dir_y = fabs ( ball_dir_y ); 
	} 

	vec2_norm ( ball_dir_x, ball_dir_y ); 
}

void run_host(int w, int h)
{
	syslog(LOG_INFO, "Running host");
	
	width = w;
	height = h;
	
	ball_pos_x = width / 2;
	ball_pos_y = height / 2;
	
	ball_dir_x = 1.0f; 
	ball_dir_y = 0.5f;
	
	racket_right_x = width - racket_width - 10;
	racket_right_y = 50.0f; 
	
	char buffer_out[message_info_size_host];
	char buffer_in[message_info_size_client];
	
	host_info_t *info_ptr = (host_info_t *)buffer_out;
	client_info_t *client_info_ptr = (client_info_t *)buffer_in;
	
	conn.reset(new conn_t(1));
	sem_t* sem_c = sem_open("pong_client", O_CREAT, 0777, 1);
	
	struct timespec ts;
	
	if(sem_c == SEM_FAILED)
	{
		syslog(LOG_ERR, "client error: %s", strerror(errno));
		return;
	}
	
	sem_t* sem_h = sem_open("pong_host", O_CREAT, 0777, 1);
	
	if(sem_h == SEM_FAILED)
	{
		syslog(LOG_ERR, "client error: %s", strerror(errno));
		return;
	}
	
	while(true)
	{
		info_ptr->ball_pos_x = ball_pos_x;
		info_ptr->ball_pos_y = ball_pos_y;
		
		info_ptr->racket_right_y = racket_right_y;
		info_ptr->racket_right_x = racket_right_x; 
		
		int ret_val = TEMP_FAILURE_RETRY(conn->write(buffer_out, message_info_size_host + 1));	
		if(ret_val == -1)
		{
			syslog(LOG_ERR, "host conn error: %s", strerror(errno));
		}
		int ret_sem = TEMP_FAILURE_RETRY(sem_post(sem_c));
		if(ret_sem == -1)
		{
			syslog(LOG_ERR, "host semaphore error: %s", strerror(errno));
		}
		
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 5;
		ret_sem = TEMP_FAILURE_RETRY(sem_timedwait(sem_h, &ts));
		if(ret_sem == -1)
		{
			if(errno == ETIMEDOUT)
			{
				syslog(LOG_ERR, "host timedout");
				return;
			}
			syslog(LOG_ERR, "host semaphore error: %s", strerror(errno));
		}
		ret_val = TEMP_FAILURE_RETRY(conn->read(buffer_in, message_info_size_host + 1));
		
		if(ret_val == -1)
		{
			syslog(LOG_ERR, "host conn error: %s", strerror(errno));
		}

		racket_left_x = client_info_ptr->racket_left_x;
		racket_left_y = client_info_ptr->racket_left_y;
		
		update_ball();
		update_racket_right(ball_pos_y);
		
		usleep(2500);
	}
}
