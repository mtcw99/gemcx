#ifndef GOPHER_CLIENT_H
#define GOPHER_CLIENT_H

#include "util/socket.h"

struct gopher_Client
{
	struct util_socket_Host host;
};

void gopher_Client_init(struct gopher_Client *client, const char *url);
void gopher_Client_deinit(struct gopher_Client *client);

#endif // GOPHER_CLIENT_H

