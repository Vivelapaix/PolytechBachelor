#include "conn.h"
#include "hash.h"
#include <string.h>
#include <cstring>
#include "host.h"
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <syslog.h>

static void *shared_memory_send;
static void *shared_memory_recieve;

conn_t::conn_t(bool isHost)
{
	int send_id = shmget(get_hash(getSenderStr(isHost)), 2 * message_info_size_host, 0);
	int recieve_id = shmget(get_hash(getRecieverStr(isHost)), 2 * message_info_size_host, 0);
	
	shared_memory_send = shmat(send_id, NULL, 0);
	shared_memory_recieve = shmat(recieve_id, NULL, 0);
}

int conn_t::write(char *buf , size_t size)
{
	memcpy(shared_memory_send, (void*) buf, size);
	return 0;
}

int conn_t::read(char *buf , size_t size)
{
	memcpy((void*) buf, shared_memory_recieve, size);
	return 0;
}

conn_t::~conn_t()
{
	shmdt(shared_memory_send);
	shmdt(shared_memory_recieve);
	syslog(LOG_INFO, "conn destroyed");
}
