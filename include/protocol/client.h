#ifndef PROTOCOL_CLIENT_H
#define PROTOCOL_CLIENT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "util/socket.h"

struct protocol_Client
{
	enum protocol_Type type;

	struct util_socket_Host host;
	char localTmpPath[256];
	char url[1024];
};

void protocol_Client_newUrl(struct protocol_Client *client, const char *url);
int32_t protocol_Client_request(struct protocol_Client *client,
		FILE *fp);

void protocol_Client_printInfo(const struct protocol_Client *client);
const char *protocol_Client_getErrorStr(const struct protocol_Client *client,
		const int32_t error);

#endif // PROTOCOL_CLIENT_H

