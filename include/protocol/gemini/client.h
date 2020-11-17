#ifndef PROTOCOL_GEMINI_CLIENT_H
#define PROTOCOL_GEMINI_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "util/socket.h"
#include "protocol/client.h"

struct p_gemini_TLS
{
	SSL_CTX *ctx;
	BIO *bio;
	BIO *ssl_bio;
	SSL *ssl;

	long res;
	int fd;

	const long flags;
};

enum p_gemini_Client_ConnectError
{
	P_GEMINI_CLIENT_CONNECTERROR_NONE = 0,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_WEB,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_WEBNOCONN,
	P_GEMINI_CLIENT_CONNECTERROR_SOCKETCONN,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE,
	P_GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT,
	P_GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT,

	P_GEMINI_CLIENT_CONNECTERROR__TOTAL
};

void p_gemini_Client_GINIT(void);

enum p_gemini_Client_ConnectError p_gemini_Client_request(struct protocol_Client *client,
		FILE *fp);
void p_gemini_Client_printInfo(const struct protocol_Client *client);
const char *p_gemini_Client_getErrorStr(const enum p_gemini_Client_ConnectError error);

#endif // PROTOCOL_GEMINI_CLIENT_H

