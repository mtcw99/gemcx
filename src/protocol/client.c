#include "protocol/client.h"

#include "protocol/gemini/client.h"
#include "protocol/gopher/client.h"

#include "util/ex.h"

#include <string.h>

static bool ginitUsed = false;

static void
protocol_Client__GINIT(void)
{
	if (!ginitUsed)
	{
		p_gemini_Client_GINIT();
		ginitUsed = true;
	}
}

void
protocol_Client_newUrl(struct protocol_Client *client,
		const char *url)
{
	protocol_Client__GINIT();

	util_socket_Host_init(&client->host, url);
	strcpy(client->url, url);
	client->localTmpPath[0] = '\0';

	// Link sanatization
	util_ex_rmchs(client->host.scheme, strlen(client->host.scheme), " ",
			true);

	// Change type based on host scheme
	if (!strcmp(client->host.scheme, "gemini"))
	{
		client->type = PROTOCOL_TYPE_GEMINI;
		printf("protocol/Client/newUrl: Type: Gemini\n");
	}
	else if (!strcmp(client->host.scheme, "gopher"))
	{
		client->type = PROTOCOL_TYPE_GOPHER;
		printf("protocol/Client/newUrl: Type: Gopher\n");
	}
	else if (!strcmp(client->host.scheme, "file"))
	{
		client->type = PROTOCOL_TYPE_FILE;
		printf("protocol/Client/newUrl: Type: File\n");
	}
	else
	{
		fprintf(stderr, "ERROR: Unsupported scheme '%s'!\n",
				client->host.scheme);
	}
}

int32_t
protocol_Client_request(struct protocol_Client *client,
		FILE *fp)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		return p_gemini_Client_request(client, fp);
	case PROTOCOL_TYPE_GOPHER:
		return p_gopher_Client_request(client, fp);
	default:
		protocol_Type_assert(client->type);
	}
	return 0;
}

char *
protocol_Client_constructUrl(struct protocol_Client *client)
{
	return util_socket_Host_constructUrl(&client->host);
}

void
protocol_Client_printInfo(const struct protocol_Client *client)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Client_printInfo(client);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Client_printInfo(client);
		break;
	default:
		protocol_Type_assert(client->type);
	}

}

const char *
protocol_Client_getErrorStr(const struct protocol_Client *client,
		const int32_t error)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		return p_gemini_Client_getErrorStr(error);
	case PROTOCOL_TYPE_GOPHER:
		return p_gopher_Client_getErrorStr(error);
	default:
		protocol_Type_assert(client->type);
	}
	return NULL;
}

