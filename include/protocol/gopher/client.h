#ifndef PROTOCOL_GOPHER_CLIENT_H
#define PROTOCOL_GOPHER_CLIENT_H

#include "util/socket.h"
#include "protocol/client.h"

#include <stdio.h>

enum p_gopher_Client_ConnectError
{
	P_GOPHER_CLIENT_CONNECTERROR_NONE = 0,
	P_GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN,
	P_GOPHER_CLIENT_CONNECTERROR_NOSEND,
	P_GOPHER_CLIENT_CONNECTERROR_NORECV,

	P_GOPHER_CLIENT_CONNECTERROR__TOTAL
};

enum p_gopher_Client_ConnectError p_gopher_Client_request(struct protocol_Client *client,
		FILE *fp);
void p_gopher_Client_printInfo(const struct protocol_Client *client);
const char *p_gopher_Client_getErrorStr(const enum p_gopher_Client_ConnectError error);

#endif // PROTOCOL_GOPHER_CLIENT_H

