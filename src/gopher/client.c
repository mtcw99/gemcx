#include "gopher/client.h"

void
gopher_Client_init(struct gopher_Client *client, const char *url)
{
	util_socket_Host_init(&client->host, url);
}

void
gopher_Client_deinit(struct gopher_Client *client)
{
}

