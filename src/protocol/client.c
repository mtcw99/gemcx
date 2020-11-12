#include "protocol/client.h"

void
protocol_Client_GINIT(void)
{
	p_gemini_Client_GINIT();
}

inline void
protocol_Client_init(struct protocol_Client *client,
		const enum protocol_Type type)
{
	client->type = type;
}

void
protocol_Client_newUrl(struct protocol_Client *client, const char *url)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Client_init(&client->protocol.gemini, url);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Client_init(&client->protocol.gopher, url);
		break;
	default:
		protocol_Type_assert(client->type);
	}
}

void
protocol_Client_deinit(struct protocol_Client *client)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Client_deinit(&client->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Client_deinit(&client->protocol.gopher);
		break;
	default:
		protocol_Type_assert(client->type);
	}
}

int32_t
protocol_Client_request(struct protocol_Client *client,
		FILE *fp)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		return p_gemini_Client_request(&client->protocol.gemini, fp);
	case PROTOCOL_TYPE_GOPHER:
		return p_gopher_Client_request(&client->protocol.gopher, fp);
	default:
		protocol_Type_assert(client->type);
	}
	return 0;
}

void
protocol_Client_printInfo(const struct protocol_Client *client)
{
	switch (client->type)
	{
	case PROTOCOL_TYPE_GEMINI:
		p_gemini_Client_printInfo(&client->protocol.gemini);
		break;
	case PROTOCOL_TYPE_GOPHER:
		p_gopher_Client_printInfo(&client->protocol.gopher);
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

