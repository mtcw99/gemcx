#ifndef PROTOCOL_GOPHER_CLIENT_H
#define PROTOCOL_GOPHER_CLIENT_H

#include "util/socket.h"

#include <stdio.h>

struct p_gopher_Client
{
	struct util_socket_Host host;
};

enum p_gopher_Client_ConnectError
{
	P_GOPHER_CLIENT_CONNECTERROR_NONE = 0,
	P_GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN,
	P_GOPHER_CLIENT_CONNECTERROR_NOSEND,
	P_GOPHER_CLIENT_CONNECTERROR_NORECV,

	P_GOPHER_CLIENT_CONNECTERROR__TOTAL
};

void p_gopher_Client_init(struct p_gopher_Client *client, const char *url);
void p_gopher_Client_deinit(struct p_gopher_Client *client);
enum p_gopher_Client_ConnectError p_gopher_Client_request(struct p_gopher_Client *client,
		FILE *fp);
void p_gopher_Client_printInfo(const struct p_gopher_Client *client);
const char *p_gopher_Client_getErrorStr(const enum p_gopher_Client_ConnectError error);

#endif // PROTOCOL_GOPHER_CLIENT_H

