#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "util/socket.h"

struct gemini_Client
{
	struct util_socket_Host host;
	char localTmpPath[256];
	char url[1024];
};

struct gemini_TLS
{
	SSL_CTX *ctx;
	BIO *bio;
	BIO *ssl_bio;
	SSL *ssl;

	long res;
	int fd;

	const long flags;
};

enum gemini_Client_ConnectError
{
	GEMINI_CLIENT_CONNECTERROR_NONE = 0,
	GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT,
	GEMINI_CLIENT_CONNECTERROR_TLS_WEB,
	GEMINI_CLIENT_CONNECTERROR_TLS_WEBNOCONN,
	GEMINI_CLIENT_CONNECTERROR_SOCKETCONN,
	GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL,
	GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN,
	GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME,
	GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD,
	GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN,
	GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE,
	GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT,
	GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT,

	GEMINI_CLIENT_CONNECTERROR__TOTAL
};

void gemini_Client_GINIT(void);

void gemini_Client_init(struct gemini_Client *client, const char *uri);
void gemini_Client_deinit(struct gemini_Client *client);
enum gemini_Client_ConnectError gemini_Client_request(struct gemini_Client *client,
		FILE *fp);
void gemini_Client_printInfo(const struct gemini_Client *client);
const char *gemini_Client_getErrorStr(const enum gemini_Client_ConnectError error);

#endif // GEMINI_CLIENT_H

