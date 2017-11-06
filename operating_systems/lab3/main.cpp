#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "conn.h"
#include "render.h"
#include "client.h"
#include "host.h"
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <mqueue.h>
#include <fcntl.h> 
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

pid_t child_pid;
void * shared_memory_client;
void * shared_memory_host;

int pipe_write_host;
int pipe_write_client;
int pipe_read_host;
int pipe_read_client;

void on_quit()
{
	sem_t* sem_c = sem_open("pong_client", 0);
	sem_t* sem_h = sem_open("pong_host", 0);
	sem_destroy(sem_c);
	sem_destroy(sem_h);
	munmap(shared_memory_host, message_info_size_host + 1);
	munmap(shared_memory_client, message_info_size_client + 1);
	close(pipe_read_client);
	close(pipe_read_host);
	close(pipe_write_host);
	close(pipe_write_client);
	unlink("host");
	unlink("client");
	syslog(LOG_INFO, "Client closed");
	kill(child_pid, SIGTERM);
	exit (EXIT_SUCCESS);
}

void sigterm_callback(int)
{
	syslog(LOG_INFO, "Host closed");
	closelog();
	exit (EXIT_SUCCESS);
}

int main ( int argc, char ** argv ) 
{ 
	openlog("pong", 0, LOG_LOCAL0); 
	
	shared_memory_client = mmap(NULL, message_info_size_client + 1, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
	shared_memory_host = mmap(NULL, message_info_size_host + 1, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
	
	int pipe_c[2];
	int pipe_h[2];
	
	pipe(pipe_c);
	pipe(pipe_h);
	
	pipe_write_host = pipe_h[1];
	pipe_read_host = pipe_h[0];
	pipe_write_client = pipe_c[1];
	pipe_read_client = pipe_c[0];
	
	mkfifo("host", 0777);
	mkfifo("client", 0777);
	
	child_pid = fork();
	
	//parent
	if(child_pid)
	{		
		atexit(on_quit);
		std::thread thr_client(run_client, 800, 600);
		render_init(argc, argv, 800, 600);
	}
	else
	{
		signal(SIGTERM, sigterm_callback);
		run_host(800, 600);
	}
	return 0 ; 
}

