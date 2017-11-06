#include "conn.h"
#include <string.h>
#include <cstring>
#include <syslog.h>

static void* shm_for_send;
static void* shm_for_recieve;

conn_t::conn_t(bool isHost)
{
	if(isHost)
	{
		shm_for_send = shared_memory_host;
		shm_for_recieve = shared_memory_client;
	}
	else
	{
		shm_for_send = shared_memory_client;
		shm_for_recieve = shared_memory_host;
	}
}

int conn_t::write(char *buf , size_t size)
{
	memcpy(shm_for_send, (void*) buf, size);
	return 0;
}

int conn_t::read(char *buf , size_t size)
{
	memcpy((void*) buf, shm_for_recieve, size);
	return 0;
}

conn_t::~conn_t()
{
	syslog(LOG_INFO, "conn destroyed");
}
