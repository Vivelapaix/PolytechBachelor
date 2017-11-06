#include "conn.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h> 

static int send_fd;
static int recieve_fd;

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
	
		recieve_fd = TEMP_FAILURE_RETRY(open(getRecieverStr(isHost), O_RDONLY));
		if(send_fd == -1)
		{
			syslog(LOG_ERR, "recieve %s %s", strerror(errno), getRecieverStr(isHost));
			exit (EXIT_FAILURE);
		}
	
		send_fd = TEMP_FAILURE_RETRY(open(getSenderStr(isHost), O_WRONLY));
		if(send_fd == -1)
		{
			syslog(LOG_ERR, "send %s %s", strerror(errno), getSenderStr(isHost));
			exit (EXIT_FAILURE);
		}
	
	}
	else
	{
		send_fd = TEMP_FAILURE_RETRY(open(getSenderStr(isHost), O_WRONLY));
		if(send_fd == -1)
		{
			syslog(LOG_ERR, "send %s %s", strerror(errno), getSenderStr(isHost));
			exit (EXIT_FAILURE);
		}
		
		recieve_fd = TEMP_FAILURE_RETRY(open(getRecieverStr(isHost), O_RDONLY));
		if(send_fd == -1)
		{
			syslog(LOG_ERR, "recieve %s %s", strerror(errno), getRecieverStr(isHost));
			exit (EXIT_FAILURE);
		}
	}
	
}

int conn_t::write(char *buf , size_t size)
{
	return write_ex(send_fd, (void*)buf, size);
}

int conn_t::read(char *buf , size_t size)
{
	return read_ex(recieve_fd, (void*)buf, size);
}

conn_t::~conn_t()
{
	close(send_fd);
	close(recieve_fd);
	syslog(LOG_INFO, "conn destroyed");
}
