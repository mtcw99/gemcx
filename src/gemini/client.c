#include "gemini/client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

const char *GEMINI_PORT = "1965";

enum gemini_Client_UriFind
{
	GEMINI_CLIENT_URIFIND_PROTOCOL = 0,
	GEMINI_CLIENT_URIFIND_HOSTNAME,
	GEMINI_CLIENT_URIFIND_RESOURCE
};

static const char *geminiClientErrorStr[GEMINI_CLIENT_CONNECTERROR__TOTAL] = {
	[GEMINI_CLIENT_CONNECTERROR_NONE] = "None",
	[GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT] = "TLS: Context unable to initalize",
	[GEMINI_CLIENT_CONNECTERROR_TLS_WEB] = "TLS: BIO not initalize",
	[GEMINI_CLIENT_CONNECTERROR_TLS_WEBNOCONN] = "TLS: BIO not connected",
	[GEMINI_CLIENT_CONNECTERROR_SOCKETCONN] = "Socket: Socket not connected",
	[GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL] = "TLS: No SSL",
	[GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN] = "TLS: Can't set1",
	[GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME] = "TLS: Can't set hostname",
	[GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD] = "TLS: Can't set fd",
	[GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN] = "TLS: No SSL connection",
	[GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE] = "TLS: Invalid peer certificate",
	[GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT] = "TLS: Cannot verify result",
	[GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT] = "TLS: No request sent",
};

static enum gemini_Client_ConnectError
gemini_Client__cleanupRetErr(struct gemini_TLS *tls,
		const enum gemini_Client_ConnectError error)
{
	if (tls->bio)
	{
		BIO_free_all(tls->bio);
		tls->bio = NULL;
	}

	if (tls->ssl != NULL)
	{
		SSL_free(tls->ssl);
	}

	if (tls->ctx != NULL)
	{
		SSL_CTX_free(tls->ctx);
	}

	if (tls->fd != -1)
	{
		close(tls->fd);
		tls->fd = -1;
	}

	return error;
}

void
gemini_Client_init(struct gemini_Client *client, const char *uri)
{
	const uint32_t uriSize = strlen(uri);
	enum gemini_Client_UriFind uriFind = GEMINI_CLIENT_URIFIND_PROTOCOL;

	char proto[256] = { 0 };
	uint32_t protoSize = 0;
	bool protoRead = true;

	uint32_t hostnameSize = 1;
	uint32_t resourceSize = 1;

	for (uint32_t i = 0; i < uriSize; ++i)
	{
		switch (uriFind)
		{
		case GEMINI_CLIENT_URIFIND_PROTOCOL:
			switch (uri[i])
			{
			case ':':
			case '/':
				protoRead = false;
				break;
			default:
				if (protoRead)
				{
					proto[protoSize++] = uri[i];
				}
				else
				{
					uriFind = GEMINI_CLIENT_URIFIND_HOSTNAME;
					proto[protoSize] = '\0';
					client->hostname[0] = uri[i];
				}
				break;
			}
			break;
		case GEMINI_CLIENT_URIFIND_HOSTNAME:
			switch (uri[i])
			{
			case '/':
				uriFind = GEMINI_CLIENT_URIFIND_RESOURCE;
				client->hostname[hostnameSize] = '\0';
				client->hostResource[0] = '/';
				break;
			default:
				client->hostname[hostnameSize++] = uri[i];
				break;
			}
			break;
		case GEMINI_CLIENT_URIFIND_RESOURCE:
			client->hostResource[resourceSize++] = uri[i];
			break;
		}
	}
	client->hostResource[resourceSize] = '\0';

	strcpy(client->scheme, proto);
	strcpy(client->url, uri);
	client->localTmpPath[0] = '\0';
}

void
gemini_Client_deinit(struct gemini_Client *client)
{
	(void) client;
}

void *
gemini_Client__getInAddr(struct sockaddr *sa)
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

static int32_t
gemini_Client__socketConn(struct gemini_Client *client, int32_t *sockfd)
{
	int32_t rv;
	struct addrinfo hints = { 0 };
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	char inAddr[INET6_ADDRSTRLEN] = { 0 };

	struct addrinfo *addrinfo;
	rv = getaddrinfo(client->hostname, GEMINI_PORT, &hints, &addrinfo);
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

	inet_ntop(p->ai_family, gemini_Client__getInAddr((struct sockaddr *) p->ai_addr),
			inAddr, sizeof(inAddr));
	printf("inAddr: %s\n", inAddr);

	freeaddrinfo(addrinfo);

	printf("gemini_Client__socketConn: Connection made\n");

	return 0;
}

int32_t
gemini_Client__verifyCallback(int32_t preverify, X509_STORE_CTX *x509_ctx)
{
	(void) preverify;
	(void) x509_ctx;
#if 0
	int32_t depth = X509_STORE_CTX_get_error_depth(x509_ctx);
	int32_t err = X509_STORE_CTX_get_error(x509_ctx);

	X509 *cert = X509_STORE_CTX_get_current_cert(x509_ctx);
	X509_NAME *iname = cert ? X509_get_issuer_name(cert) : NULL;
	X509_NAME *sname = cert ? X509_get_subject_name(cert) : NULL;
	(void) iname;
	(void) sname;
	printf("preverify: %d\n", preverify);
#endif
	return 1;
}

enum gemini_Client_ConnectError
gemini_Client_request(struct gemini_Client *client,
		FILE *fp)
{
	struct gemini_TLS tls = {
		.ctx = NULL,
		.bio = NULL,
		.ssl_bio = NULL,
		.ssl = NULL,
		.res = 1,
		.fd = -1,

		.flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION,
	};

	tls.ctx = SSL_CTX_new(TLS_method());
	if (tls.ctx == NULL)
	{
		return GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT;
	}
	SSL_CTX_set_verify(tls.ctx, SSL_VERIFY_PEER, gemini_Client__verifyCallback);

	tls.ssl_bio = BIO_new(BIO_f_ssl());
	if (tls.ssl_bio == NULL)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_WEB);
	}

#if 1
	tls.res = gemini_Client__socketConn(client, &tls.fd);
	if (tls.res == -1)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_SOCKETCONN);
	}
	//printf("tls.fd: %d\n", tls.fd);
#endif

	char hostnamePort[512] = { 0 };
	sprintf(hostnamePort, "%s:%s", client->hostname, GEMINI_PORT);
	printf("Hostname:PORT: %s\n", hostnamePort);

	tls.ssl = SSL_new(tls.ctx);
	if (tls.ssl == NULL)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL);
	}
	SSL_set_connect_state(tls.ssl);

	tls.res = SSL_set1_host(tls.ssl, client->hostname);
	if (tls.res != 1)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN);
	}

	tls.res = SSL_set_tlsext_host_name(tls.ssl, client->hostname);
	if (tls.res != 1)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME);
	}

	tls.res = SSL_set_fd(tls.ssl, tls.fd);
	if (tls.res != 1)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD);
	}

	tls.res = SSL_connect(tls.ssl);
	if (tls.res != 1)
	{
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "SSL_connect: res: %ld\n", tls.res);
		int32_t ssl_error = SSL_get_error(tls.ssl, tls.res);
		fprintf(stderr, "ssl_error: %d\n", ssl_error);
		switch (ssl_error)
		{
		case SSL_ERROR_NONE:
			break;
		case SSL_ERROR_ZERO_RETURN:
			fprintf(stderr, "SSL_ERROR_ZERO_RETURN\n");
			break;
		case SSL_ERROR_WANT_READ:
			fprintf(stderr, "SSL_ERROR_WANT_READ\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			fprintf(stderr, "SSL_ERROR_WANT_WRITE\n");
			break;
		case SSL_ERROR_WANT_CONNECT:
			fprintf(stderr, "SSL_ERROR_WANT_CONNECT\n");
			break;
		case SSL_ERROR_WANT_ACCEPT:
			fprintf(stderr, "SSL_ERROR_WANT_ACCEPT\n");
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			fprintf(stderr, "SSL_ERROR_WANT_X509_LOOKUP\n");
			break;
		case SSL_ERROR_SYSCALL:
			fprintf(stderr, "SSL_ERROR_SYSCALL\n");
			break;
		case SSL_ERROR_SSL:
			fprintf(stderr, "SSL_ERROR_SSL\n");
			fprintf(stderr, "ERR_error_string: %s\n",
					ERR_error_string(
						ssl_error, NULL));
			break;
		default:
			fprintf(stderr, "ERR unknown\n");
			break;
		}
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN);
	}

	// Validates server certificate
	X509 *cert = SSL_get_peer_certificate(tls.ssl);
	if (cert == NULL)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE);
	}
	X509_free(cert);

#if 0
	long vr = SSL_get_verify_result(tls.ssl);
	if (vr != X509_V_OK)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT);
	}
#endif

	BIO_set_ssl(tls.ssl_bio, tls.ssl, 0);

	tls.bio = BIO_new(BIO_f_buffer());
	BIO_push(tls.bio, tls.ssl_bio);

	printf("send request...\n");
	// Sends request (one CRLF terminated line)
	char slRequest[2560] = { 0 };
	sprintf(slRequest, "%s\r\n", client->url);
	uint32_t slRequestLen = strlen(slRequest);
	tls.res = BIO_write(tls.ssl_bio, slRequest, slRequestLen);
	if (tls.res == -1)
	{
		return gemini_Client__cleanupRetErr(&tls,
				GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT);
	}

	printf("handle response...\n");

	// Handles response
	int32_t len = 0;
	do
	{
		char respBuf[1024 + 3] = { 0 };
		len = BIO_gets(tls.bio, respBuf, sizeof(respBuf));

		if (len > 0)
		{
			fprintf(fp, "%s", respBuf);
			//printf("respBuf: %s\n", respBuf);
		}
#if 1
	} while (len > 0 || BIO_should_retry(tls.bio));
#else
	} while (0);
#endif

	return gemini_Client__cleanupRetErr(&tls,
			GEMINI_CLIENT_CONNECTERROR_NONE);
}

void
gemini_Client_printInfo(const struct gemini_Client *client)
{
	printf("gemini client URI info:\n"
			"\tScheme: %s\n"
			"\tHostname: %s\n"
			"\tResource: %s\n\n",
			client->scheme, client->hostname,
			client->hostResource);
}

const char *
gemini_Client_getErrorStr(const enum gemini_Client_ConnectError error)
{
	return geminiClientErrorStr[error];
}

void
gemini_Client_GINIT(void)
{
	SSL_load_error_strings();
}

