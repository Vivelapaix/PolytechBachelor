#include "client.h"
#include "host.h"
#include "render.h"
#include "conn.h"
#include <syslog.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <memory>
#include <time.h>

static float racket_left_x; 
static float racket_left_y;

static int width;
static int height;

static std::unique_ptr<conn_t> conn;

void update_racket_left(float ball_position_y)
{
	if(racket_left_y + racket_height / 2 > ball_position_y + 0.5)
		racket_left_y -= racket_speed;
	if(racket_left_y + racket_height / 2 < ball_position_y - 0.5)
		racket_left_y += racket_speed;
		
	if(racket_left_y < 0.0f)
		racket_left_y = 0.0f;
	if(racket_left_y > height - racket_height)
		racket_left_y = height - racket_height;
}

void run_client(int w, int h)
{
	syslog(LOG_INFO, "Running client");
	
	width = w;
	height = h;
	
	racket_left_x = 10.0f; 
	racket_left_y = 50.0f;
	
	char buffer_out[message_info_size_client];
	char buffer_in[message_info_size_host];
	
	host_info_t *info_ptr = (host_info_t *)buffer_in;
	client_info_t *client_info_ptr = (client_info_t *)buffer_out;
	
	struct timespec ts;
	
	conn.reset(new conn_t(0));
	sem_t* sem_c = sem_open("pong_client", O_CREAT, 0777, 1);
	
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
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 5;
		int ret_sem = TEMP_FAILURE_RETRY(sem_timedwait(sem_c, &ts));
		if(ret_sem == -1)
		{
			if(errno == ETIMEDOUT)
			{
				syslog(LOG_ERR, "client timedout");
				return;
			}
			syslog(LOG_ERR, "client semaphore error: %s", strerror(errno));
		}
		int ret_val = TEMP_FAILURE_RETRY(conn->read(buffer_in, message_info_size_host + 1));
		
		if(ret_val == -1)
		{
			syslog(LOG_ERR, "client conn error: %s", strerror(errno));
		}
		
		set_racket_left(racket_left_x, racket_left_y);
		set_racket_right(info_ptr->racket_right_x, info_ptr->racket_right_y);
		set_ball(info_ptr->ball_pos_x, info_ptr->ball_pos_y);
		
		update_racket_left(info_ptr->ball_pos_y);
		
		
		client_info_ptr->racket_left_x = racket_left_x;
		client_info_ptr->racket_left_y = racket_left_y;
		
		ret_val = TEMP_FAILURE_RETRY(conn->write(buffer_out, message_info_size_client + 1));
		if(ret_val == -1)
		{
			syslog(LOG_ERR, "client conn error: %s", strerror(errno));
		}
		
		ret_sem = TEMP_FAILURE_RETRY(sem_post(sem_h));
		if(ret_sem == -1)
		{
			syslog(LOG_ERR, "client semaphore error: %s", strerror(errno));
		}
	}
}
