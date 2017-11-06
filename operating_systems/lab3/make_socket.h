#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int make_named_socket (const char *filename)
{
	struct sockaddr_un name;
	int sock;

  /* Create the socket. */
	sock = socket (PF_LOCAL, SOCK_STREAM, 0);

  /* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strcpy (name.sun_path, filename);
	
	unlink(filename);
	if(bind (sock, (struct sockaddr *) &name, SUN_LEN(&name)) < 0)
	{
		syslog(LOG_ERR, "bind fail: %s", strerror(errno));
	}

	return sock;
}
