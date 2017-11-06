#include "conn.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <syslog.h>

static int pipe_send;
static int pipe_recieve;

ssize_t write_ex(int f, void * b, size_t s)
{
	return write(f, b, s);
}

ssize_t read_ex(int f, void * b, size_t s)
{
	return read(f, b, s);
}

conn_t::conn_t(bool isHost)
{
	if(isHost)
	{
		pipe_send = pipe_write_host;
		pipe_recieve = pipe_read_client;
	}
	else
	{
		pipe_send = pipe_write_client;
		pipe_recieve = pipe_read_host;
	}
}

int conn_t::write(char *buf , size_t size)
{
	return write_ex(pipe_send, (void*)buf, size);
}

int conn_t::read(char *buf , size_t size)
{
	return read_ex(pipe_recieve, (void*)buf, size);
}

conn_t::~conn_t()
{
	syslog(LOG_INFO, "conn destroyed");
}
