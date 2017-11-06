#include "conn.h"
#include "host.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

static mqd_t mqd_send_;
static mqd_t mqd_recieve_;	

conn_t::conn_t(bool isHost)
{
	mq_attr attr;  
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 1;  
	attr.mq_msgsize = message_info_size_host + 1;  
	attr.mq_curmsgs = 0;  
	
	std::string send_name = "/";
	send_name += getSenderStr(isHost);
	std::string recieve_name = "/";
	recieve_name += getRecieverStr(isHost);
	

	mqd_send_ = mq_open(send_name.c_str(), O_NONBLOCK | O_WRONLY | (isHost ? 0 : O_CREAT), 0777, &attr);
	mqd_recieve_ = mq_open(recieve_name.c_str(), O_NONBLOCK | O_RDONLY | (isHost ? 0 : O_CREAT), 0777, &attr);
	
	if(mqd_send_ == -1)
	{
		syslog(LOG_ERR, "%s", strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	if(mqd_recieve_ == -1)
	{
		syslog(LOG_ERR, "%s", strerror(errno));
		exit (EXIT_FAILURE);
	}
}

int conn_t::write(char *buf , size_t size)
{
	return mq_send(mqd_send_, buf, size, 1);
}

int conn_t::read(char *buf , size_t size)
{
	return mq_receive(mqd_recieve_, buf, size, 0);
}

conn_t::~conn_t()
{
	mq_close(mqd_send_);
	mq_close(mqd_recieve_);
	syslog(LOG_INFO, "conn destroyed");
}
