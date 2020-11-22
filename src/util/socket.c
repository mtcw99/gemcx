#include "util/socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>

enum util_socket_UrlFind 
{
	UTIL_SOCKET_URIFIND_PROTOCOL = 0,
	UTIL_SOCKET_URIFIND_HOSTNAME,
	UTIL_SOCKET_URIFIND_RESOURCE
};

bool
util_socket_urlHasScheme(const char *url,
		const uint32_t urlSize)
{
	for (uint32_t i = 2; i < urlSize; ++i)
	{
		if (url[i - 2] == ':' &&
				url[i - 1] == '/' &&
				url[i] == '/')
		{
			return true;
		}
	}
	return false;
}

void
util_socket_Host_init(struct util_socket_Host *host, const char *url)
{
	const uint32_t urlSize = strlen(url);
	bool schemeRead = util_socket_urlHasScheme(url, urlSize);

	// Clear all names to zeros
	memset(host->scheme, '\0', sizeof(host->scheme));
	memset(host->hostname, '\0', sizeof(host->hostname));
	memset(host->resource, '\0', sizeof(host->resource));

	if (!schemeRead)
	{
		// Assume default protocol: gemini
		memset(host->scheme, 0, sizeof(host->scheme));
		strcpy(host->scheme, "gemini");
	}

	enum util_socket_UrlFind urlFind = (schemeRead) ?
			UTIL_SOCKET_URIFIND_PROTOCOL :
			UTIL_SOCKET_URIFIND_HOSTNAME;

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

	// No resource found, assume '/'
	if (resourceSize == 0)
	{
		host->resource[0] = '/';
		host->resource[1] = '\0';
	}
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

char *
util_socket_Host_constructUrl(const struct util_socket_Host *host)
{
	static char url[1024] = { 0 };
	sprintf(url, "%s://%s%s", host->scheme, host->hostname, host->resource);
	return url;
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

	const uint32_t timeoutSec = 5;
	int32_t flags = 0;

	struct addrinfo *p;
	// Loop through all the results and connect to the first one
	for (p = addrinfo; p != NULL; p = p->ai_next)
	{
		if (p->ai_family != AF_INET &&
				p->ai_family != AF_INET6)
		{
			continue;
		}

		*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (*sockfd == -1)
		{
			continue;
		}

		flags = fcntl(*sockfd, F_GETFL, 0);
		fcntl(*sockfd, F_SETFL, flags | O_NONBLOCK);

		if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			struct timespec deadline = {
				.tv_sec = now.tv_sec + timeoutSec,
				.tv_nsec = now.tv_nsec
			};
			int32_t rc = 0;

			do
			{
				clock_gettime(CLOCK_MONOTONIC, &now);
				int32_t msToDeadline = (int32_t)
						(((deadline.tv_sec - now.tv_sec) * 1000l) +
						 ((deadline.tv_nsec - now.tv_nsec) / 1000000l));
				if (msToDeadline < 0)
				{
					rc = 0;
					break;
				}

				struct pollfd pfds[] = {
					{ . fd = *sockfd, .events = POLLOUT }
				};

				rc = poll(pfds, 1, msToDeadline);

				if (rc > 0)
				{
					int32_t error = 0;
					socklen_t len = sizeof(error);
					if (getsockopt(*sockfd, SOL_SOCKET, SO_ERROR, &error, &len) == 0)
					{
						errno = error;
					}
					if (error)
					{
						rc = -1;
					}
					else
					{
						goto connected;
					}
				}
			} while(rc == -1 && errno == EINTR);

			if (rc == 0)
			{
				errno = ETIMEDOUT;
			}
		}

		fcntl(*sockfd, F_SETFL, flags);

		close(*sockfd);
	}

connected:
	fcntl(*sockfd, F_SETFL, flags);

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

