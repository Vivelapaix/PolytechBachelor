#ifndef CLIENT_H_INCLUDED__
#define CLIENT_H_INCLUDED__
//#pragma once
#include "render.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

struct client_info_t
{
	float racket_left_x; 
	float racket_left_y;
};

const int message_info_size_client = sizeof(client_info_t);

void run_client(int, int);
#endif

