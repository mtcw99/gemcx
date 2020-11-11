#include "util/socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum util_socket_UrlFind 
{
	UTIL_SOCKET_URIFIND_PROTOCOL = 0,
	UTIL_SOCKET_URIFIND_HOSTNAME,
	UTIL_SOCKET_URIFIND_RESOURCE
};

void
util_socket_Host_init(struct util_socket_Host *host, const char *url)
{
	const uint32_t urlSize = strlen(url);
	enum util_socket_UrlFind urlFind = UTIL_SOCKET_URIFIND_PROTOCOL;

	bool schemeRead = true;

	uint32_t schemeSize = 0;
	uint32_t hostnameSize = 0;
	uint32_t resourceSize = 0;

	for (uint32_t i = 0; i < urlSize; ++i)
	{
		switch (urlFind)
		{
		case UTIL_SOCKET_URIFIND_PROTOCOL:
			switch (url[i])
			{
			case ':':
			case '/':
				schemeRead = false;
				break;
			default:
				if (schemeRead)
				{
					host->scheme[schemeSize++] = url[i];
				}
				else
				{
					urlFind = UTIL_SOCKET_URIFIND_HOSTNAME;
					host->scheme[schemeSize] = '\0';
					host->hostname[hostnameSize++] = url[i];
				}
				break;
			}
			break;
		case UTIL_SOCKET_URIFIND_HOSTNAME:
			switch (url[i])
			{
			case '/':
				urlFind = UTIL_SOCKET_URIFIND_RESOURCE;
				host->hostname[hostnameSize] = '\0';
				host->resource[resourceSize++] = '/';
				break;
			default:
				host->hostname[hostnameSize++] = url[i];
				break;
			}
			break;
		case UTIL_SOCKET_URIFIND_RESOURCE:
			host->resource[resourceSize++] = url[i];
			break;
		}
	}
	host->resource[resourceSize] = '\0';
}

void
util_socket_Host_printInfo(const struct util_socket_Host *host)
{
	printf("Socket host URL info:\n"
			"\tScheme: %s\n"
			"\tHostname: %s\n"
			"\tResource: %s\n\n",
			host->scheme, host->hostname,
			host->resource);
}

void *
util_socket_getInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}
	else
	{
		return &(((struct sockaddr_in6 *) sa)->sin6_addr);
	}
}

int32_t
util_socket_connect(const char *hostname, const char *port, int32_t *sockfd)
{
	int32_t rv;
	struct addrinfo hints = { 0 };
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	char inAddr[INET6_ADDRSTRLEN] = { 0 };

	struct addrinfo *addrinfo;
	rv = getaddrinfo(hostname, port, &hints, &addrinfo);
	if (rv != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	struct addrinfo *p;
	// Loop through all the results and connect to the first one
	for (p = addrinfo; p != NULL; p = p->ai_next)
	{
		*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (*sockfd == -1)
		{
			continue;
		}

		if (connect(*sockfd, p->ai_addr, p->ai_addrlen) != -1)
		{
			break;	// Connection made
		}

		close(*sockfd);
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return -1;
	}

	inet_ntop(p->ai_family, util_socket_getInAddr((struct sockaddr *) p->ai_addr),
			inAddr, sizeof(inAddr));
	printf("inAddr: %s\n", inAddr);

	freeaddrinfo(addrinfo);

	printf("util_socket_connect: Connection made\n");

	return 0;
}

