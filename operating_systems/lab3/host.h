#ifndef HOST_H_INCLUDED__
#define HOST_H_INCLUDED__
//#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

const int racket_width = 10; 
const int racket_height = 80; 
const int racket_speed = 4;

const int ball_size = 8;
const int ball_speed = 4;

struct host_info_t
{
	float ball_pos_x; 
	float ball_pos_y;
	
	float racket_right_x; 
	float racket_right_y;
};

const int message_info_size_host = sizeof(host_info_t);

void run_host(int, int);
#endif
