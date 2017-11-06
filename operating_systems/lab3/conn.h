#ifndef CONN_H_INCLUDED__
#define CONN_H_INCLUDED__

//#pragma once
#include <sys/types.h>

extern void * shared_memory_client;
extern void * shared_memory_host;

extern int pipe_write_host;
extern int pipe_write_client;
extern int pipe_read_host;
extern int pipe_read_client;

//char const * getSenderStr(bool isHost);
//char const * getRecieverStr(bool isHost);



class conn_t
{
private:
static char const * getSenderStr(bool isHost)
{
  return isHost ? "host" : "client";
}

static char const * getRecieverStr(bool isHost)
{
  return isHost ? "client" : "host";
}

public:
	conn_t(bool isHost);
	int read(char *buf , size_t size);
	int write(char *buf , size_t size);
	~conn_t();
};



#endif
