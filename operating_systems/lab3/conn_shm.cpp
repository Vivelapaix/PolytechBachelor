#include "conn.h"
#include <sys/mman.h>
#include <sys/shm.h>
#include <string.h>
#include <cstring>
#include "host.h"
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include "hash.h"

static void *shared_memory_send;
static void *shared_memory_recieve;

static char send_name[80];
static char recieve_name[80];

static int client_shm_id, host_shm_id;

conn_t::conn_t(bool isHost)
{	
	memcpy(send_name, getSenderStr(isHost), 80);
	memcpy(recieve_name, getRecieverStr(isHost), 80);
	
	client_shm_id = shmget(get_hash("client"), 2 * message_info_size_host, 0777 | IPC_CREAT);
	host_shm_id = shmget(get_hash("host"), 2 * message_info_size_host, 0777 | IPC_CREAT);
	
	int fd_send = shm_open(getSenderStr(isHost), O_RDWR | O_CREAT | O_NONBLOCK, 0777);
	if(fd_send == -1)
	{
		syslog(LOG_ERR, "%s", strerror(errno));
		exit (EXIT_FAILURE);
	}
	ftruncate( fd_send, (size_t) sysconf (_SC_PAGESIZE) );
	
	int fd_recieve = shm_open(getRecieverStr(isHost), O_RDWR | O_CREAT | O_NONBLOCK, 0777);
	if(fd_recieve == -1)
	{
		syslog(LOG_ERR, "%s", strerror(errno));
		exit (EXIT_FAILURE);
	}
	ftruncate( fd_recieve, (size_t) sysconf (_SC_PAGESIZE) );
	
	shared_memory_send = mmap(NULL, message_info_size_host, PROT_READ|PROT_WRITE, 
							  MAP_FILE|MAP_SHARED, fd_send, 0);
							  
	shared_memory_recieve = mmap(NULL, message_info_size_host, PROT_READ|PROT_WRITE, 
								 MAP_FILE|MAP_SHARED, fd_recieve, 0);
}

int conn_t::write(char *buf , size_t size)
{
	memcpy(shared_memory_send, (void*) buf, size);
	int ret_val = msync(shared_memory_send, size, MS_SYNC);
	if(ret_val == -1)
	{
		syslog(LOG_ERR, "%s", strerror(errno));
	}
	return 0;
}

int conn_t::read(char *buf , size_t size)
{
	memcpy((void*) buf, shared_memory_recieve, size);
	return 0;
}

conn_t::~conn_t()
{
	munmap(shared_memory_send, message_info_size_host + 1);
	munmap(shared_memory_recieve, message_info_size_host + 1);
	shm_unlink(send_name);
	shm_unlink(recieve_name);
	shmctl(client_shm_id, IPC_RMID, NULL);
	shmctl(host_shm_id, IPC_RMID, NULL);
	syslog(LOG_INFO, "conn destroyed");
}
