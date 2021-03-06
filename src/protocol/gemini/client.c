#include "protocol/gemini/client.h"

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

static const char *geminiClientErrorStr[P_GEMINI_CLIENT_CONNECTERROR__TOTAL] = {
	[P_GEMINI_CLIENT_CONNECTERROR_NONE] = "None",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT] = "TLS: Context unable to initalize",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_WEB] = "TLS: BIO not initalize",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_WEBNOCONN] = "TLS: BIO not connected",
	[P_GEMINI_CLIENT_CONNECTERROR_SOCKETCONN] = "Socket: Socket not connected",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL] = "TLS: No SSL",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN] = "TLS: Can't set1",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME] = "TLS: Can't set hostname",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD] = "TLS: Can't set fd",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN] = "TLS: No SSL connection",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE] = "TLS: Invalid peer certificate",
	[P_GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT] = "TLS: Cannot verify result",
	[P_GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT] = "TLS: No request sent",
};

static enum p_gemini_Client_ConnectError
p_gemini_Client__cleanupRetErr(struct p_gemini_TLS *tls,
		const enum p_gemini_Client_ConnectError error)
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

int32_t
p_gemini_Client__verifyCallback(int32_t preverify, X509_STORE_CTX *x509_ctx)
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

enum p_gemini_Client_ConnectError
p_gemini_Client_request(struct protocol_Client *client,
		FILE *fp)
{
	struct p_gemini_TLS tls = {
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
		return P_GEMINI_CLIENT_CONNECTERROR_TLS_NOCONTEXT;
	}
	SSL_CTX_set_verify(tls.ctx, SSL_VERIFY_PEER, p_gemini_Client__verifyCallback);

	tls.ssl_bio = BIO_new(BIO_f_ssl());
	if (tls.ssl_bio == NULL)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_WEB);
	}

	tls.res = util_socket_connect(client->host.hostname, GEMINI_PORT, &tls.fd);
	if (tls.res == -1)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_SOCKETCONN);
	}
	//printf("tls.fd: %d\n", tls.fd);

	char hostnamePort[512] = { 0 };
	sprintf(hostnamePort, "%s:%s", client->host.hostname, GEMINI_PORT);
	printf("Hostname:PORT: %s\n", hostnamePort);

	tls.ssl = SSL_new(tls.ctx);
	if (tls.ssl == NULL)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSSL);
	}
	SSL_set_connect_state(tls.ssl);

	tls.res = SSL_set1_host(tls.ssl, client->host.hostname);
	if (tls.res != 1)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSET1HN);
	}

	tls.res = SSL_set_tlsext_host_name(tls.ssl, client->host.hostname);
	if (tls.res != 1)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_NOSETHOSTNAME);
	}

	tls.res = SSL_set_fd(tls.ssl, tls.fd);
	if (tls.res != 1)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSETFD);
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
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_CANTSSLCONN);
	}

	// Validates server certificate
	X509 *cert = SSL_get_peer_certificate(tls.ssl);
	if (cert == NULL)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_PEERCERTIFICATE);
	}
	X509_free(cert);

#if 0
	long vr = SSL_get_verify_result(tls.ssl);
	if (vr != X509_V_OK)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_TLS_VERIFYRESULT);
	}
#endif

	BIO_set_ssl(tls.ssl_bio, tls.ssl, 0);

	tls.bio = BIO_new(BIO_f_buffer());
	BIO_push(tls.bio, tls.ssl_bio);

	printf("send request... %s\n", util_socket_Host_constructUrl(&client->host));
	// Sends request (one CRLF terminated line)
	char slRequest[2560] = { 0 };
	sprintf(slRequest, "%s\r\n", util_socket_Host_constructUrl(&client->host));
	uint32_t slRequestLen = strlen(slRequest);
	tls.res = BIO_write(tls.ssl_bio, slRequest, slRequestLen);
	if (tls.res == -1)
	{
		return p_gemini_Client__cleanupRetErr(&tls,
				P_GEMINI_CLIENT_CONNECTERROR_BIO_NOREQSENT);
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

	return p_gemini_Client__cleanupRetErr(&tls,
			P_GEMINI_CLIENT_CONNECTERROR_NONE);
}

void
p_gemini_Client_printInfo(const struct protocol_Client *client)
{
	util_socket_Host_printInfo(&client->host);
}

const char *
p_gemini_Client_getErrorStr(const enum p_gemini_Client_ConnectError error)
{
	return geminiClientErrorStr[error];
}

void
p_gemini_Client_GINIT(void)
{
	SSL_load_error_strings();
}

