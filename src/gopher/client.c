#include "gopher/client.h"

#include <string.h>

static const char *GOPHER_PORT = "70";
enum
{
	BUFFER_SIZE = 1024
};

void
gopher_Client_init(struct gopher_Client *client, const char *url)
{
	util_socket_Host_init(&client->host, url);
}

void
gopher_Client_deinit(struct gopher_Client *client)
{
}

enum gopher_Client_ConnectError
gopher_Client_request(struct gopher_Client *client,
		FILE *fp)
{
	int32_t sockfd;
	int32_t rv = util_socket_connect(client->host.hostname, GOPHER_PORT, &sockfd);
	if (rv == -1)
	{
		return GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN;
	}
	char buffer[BUFFER_SIZE] = { 0 };
	size_t bufSRLen = 0;

	sprintf(buffer, "/\r\n"); //, client->host.resource);
	bufSRLen = send(sockfd, buffer, BUFFER_SIZE, 0);
	if (bufSRLen < 0)
	{
		perror("Sending error");
		close(sockfd);
		return GOPHER_CLIENT_CONNECTERROR_NOSEND;
	}

	bool recvBuf = true;
	while (recvBuf)
	{
		memset(&buffer, 0, BUFFER_SIZE);
		bufSRLen = recv(sockfd, buffer, BUFFER_SIZE, 0);
		if (bufSRLen < 0)
		{
			perror("Receiving error");
			close(sockfd);
			return GOPHER_CLIENT_CONNECTERROR_NORECV;
		}
		else if (bufSRLen == 0)
		{
			recvBuf = false;
		}
		else
		{
			fprintf(fp, "%s", buffer);
		}
	}

	close(sockfd);

	return GOPHER_CLIENT_CONNECTERROR_NONE;
}

void
gopher_Client_printInfo(const struct gopher_Client *client)
{
	util_socket_Host_printInfo(&client->host);
}

