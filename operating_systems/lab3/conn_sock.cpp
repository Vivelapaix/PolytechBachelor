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
#include <sys/socket.h>
#include <semaphore.h>
#include "make_socket.h"

static bool is_host;
static int sock;
static int socket_host;

const char host_name[] = "/host";
const char client_name[] = "/client";

static sem_t* semaphore;

conn_t::conn_t(bool isHost)
{
	semaphore = sem_open("socket_sem", O_CREAT, 0777, 1);
	
	if(isHost)
	{	
		socket_host = make_named_socket(host_name);
		int ret_val = listen(socket_host, 1);
		if(ret_val < 0)
		{
			syslog(LOG_ERR, "listen fail: %s", strerror(errno));
			exit (EXIT_FAILURE);
		}
		TEMP_FAILURE_RETRY(sem_post(semaphore));
		
		struct sockaddr client_addr;
		socklen_t sa_len = sizeof(struct sockaddr);

		sock = accept(socket_host, &client_addr, &sa_len);
		if(sock < 0)
		{
			syslog(LOG_ERR, "%s", strerror(errno));
			exit (EXIT_FAILURE);
		}
		
		is_host = true;
	}
	else
	{
		int socket_client = make_named_socket(client_name);
		socket_host = -1;
		
		struct sockaddr_un host_addr_un;
		host_addr_un.sun_family = AF_LOCAL;
		strcpy (host_addr_un.sun_path, host_name);	
		
		//struct sockaddr host_addr = (struct sockaddr) host_addr_un;
		
		TEMP_FAILURE_RETRY(sem_wait(semaphore));
		int res = connect(socket_client, (struct sockaddr*)&host_addr_un, sizeof(host_addr_un));
		if(res < 0)
		{
			syslog(LOG_ERR, "%s", strerror(errno));
			exit (EXIT_FAILURE);
		}
		
		sock = socket_client;
		is_host = false;
	}
		
	
}

int conn_t::write(char *buf , size_t size)
{
	return send(sock, buf, size, 0);
}

int conn_t::read(char *buf , size_t size)
{
	return recv(sock, buf, size, 0);
}

conn_t::~conn_t()
{
	close(sock);
	if(socket_host != -1)
		close(socket_host);
	sem_destroy(semaphore);
	syslog(LOG_INFO, "conn destroyed");
}
