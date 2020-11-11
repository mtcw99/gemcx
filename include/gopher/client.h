#ifndef GOPHER_CLIENT_H
#define GOPHER_CLIENT_H

#include "util/socket.h"

#include <stdio.h>

struct gopher_Client
{
	struct util_socket_Host host;
};

enum gopher_Client_ConnectError
{
	GOPHER_CLIENT_CONNECTERROR_NONE = 0,
	GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN,
	GOPHER_CLIENT_CONNECTERROR_NOSEND,
	GOPHER_CLIENT_CONNECTERROR_NORECV,

	GOPHER_CLIENT_CONNECTERROR__TOTAL
};

void gopher_Client_init(struct gopher_Client *client, const char *url);
void gopher_Client_deinit(struct gopher_Client *client);
enum gopher_Client_ConnectError gopher_Client_request(struct gopher_Client *client,
		FILE *fp);
void gopher_Client_printInfo(const struct gopher_Client *client);

#endif // GOPHER_CLIENT_H

