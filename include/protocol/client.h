#ifndef PROTOCOL_CLIENT_H
#define PROTOCOL_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/type.h"
#include "protocol/gemini/client.h"
#include "protocol/gopher/client.h"

struct protocol_Client
{
	enum protocol_Type type;

	union 
	{
		struct p_gemini_Client gemini;
		struct p_gopher_Client gopher;
	} protocol;
};

void protocol_Client_GINIT(void);

void protocol_Client_init(struct protocol_Client *client,
		const enum protocol_Type type);
void protocol_Client_newUrl(struct protocol_Client *client, const char *url);
void protocol_Client_deinit(struct protocol_Client *client);
int32_t protocol_Client_request(struct protocol_Client *client,
		FILE *fp);
void protocol_Client_printInfo(const struct protocol_Client *client);
const char *protocol_Client_getErrorStr(const struct protocol_Client *client,
		const int32_t error);

#endif // PROTOCOL_CLIENT_H

