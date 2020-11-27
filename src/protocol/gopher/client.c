#include "protocol/gopher/client.h"

#include <string.h>

static const char *GOPHER_PORT = "70";
enum
{
	BUFFER_SIZE = 1024
};


static const char *gopherClientErrorStr[P_GOPHER_CLIENT_CONNECTERROR__TOTAL] = {
	[P_GOPHER_CLIENT_CONNECTERROR_NONE] = "No error",
	[P_GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN] = "No socket connection",
	[P_GOPHER_CLIENT_CONNECTERROR_NOSEND] = "No request sent",
	[P_GOPHER_CLIENT_CONNECTERROR_NORECV] = "No response received"
};

enum p_gopher_Client_ConnectError
p_gopher_Client_request(struct protocol_Client *client,
		FILE *fp)
{
	int32_t sockfd;
	int32_t rv = util_socket_connect(client->host.hostname, GOPHER_PORT, &sockfd);
	if (rv == -1)
	{
		return P_GOPHER_CLIENT_CONNECTERROR_NOSOCKETCONN;
	}
	char buffer[BUFFER_SIZE] = { 0 };
	size_t bufSRLen = 0;

	if (client->host.resource[1] != '\0' && client->host.resource[2] == '/')
	{
		sprintf(buffer, "%s\r\n", client->host.resource + 2);
	}
	else
	{
		sprintf(buffer, "%s\r\n", client->host.resource);
	}
	//printf("Buffer send: %s\n", buffer);
	bufSRLen = send(sockfd, buffer, BUFFER_SIZE, 0);
	if (bufSRLen < 0)
	{
		perror("Sending error");
		close(sockfd);
		return P_GOPHER_CLIENT_CONNECTERROR_NOSEND;
	}

	bool recvBuf = true;
	while (recvBuf)
	{
		memset(&buffer, 0, BUFFER_SIZE);
		bufSRLen = recv(sockfd, buffer, BUFFER_SIZE, 0);
		buffer[bufSRLen] = '\0';
		if (bufSRLen < 0)
		{
			perror("Receiving error");
			close(sockfd);
			return P_GOPHER_CLIENT_CONNECTERROR_NORECV;
		}
		else if (bufSRLen == 0)
		{
			recvBuf = false;
		}
		else
		{
			fprintf(fp, "%s", buffer);
			//printf("BUFFER -> fp: %s\n", buffer);
		}
	}

	close(sockfd);

	return P_GOPHER_CLIENT_CONNECTERROR_NONE;
}

void
p_gopher_Client_printInfo(const struct protocol_Client *client)
{
	util_socket_Host_printInfo(&client->host);
}

const char *
p_gopher_Client_getErrorStr(const enum p_gopher_Client_ConnectError error)
{
	return gopherClientErrorStr[error];
}

