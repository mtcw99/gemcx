#ifndef UTIL_SOCKET_H
#define UTIL_SOCKET_H

#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

struct util_socket_Host
{
	char scheme[256];
	char hostname[256];
	char resource[256];
};

bool util_socket_urlHasScheme(const char *url, const uint32_t urlSize);
void util_socket_Host_init(struct util_socket_Host *host, const char *url);
void util_socket_Host_printInfo(const struct util_socket_Host *host);
char *util_socket_Host_constructUrl(const struct util_socket_Host *host);
void *util_socket_getInAddr(struct sockaddr *sa);
int32_t util_socket_connect(const char *hostname, const char *port, int32_t *sockfd);

#endif // UTIL_SOCKET_H

