#include "gemcx/xcb/connectUrl.h"

#include "protocol/header.h"
#include "gemcx/config.h"
#include "util/ex.h"

int32_t
gemcx_xcb_ConnectUrl_connect(struct protocol_Client *const restrict client,
		struct Parser *const restrict parser,
		const char *const restrict ncUrl)
{
	static bool parserHasInit = false;
	char url[1024] = { 0 };
	strcpy(url, ncUrl);
	util_ex_rmchs(url, strlen(ncUrl), "\t\n", true);
	printf("Connecting: '%s'\n", url);

	const uint32_t urlLen = strlen(url);
	struct protocol_Client tmpClient = *client;
	protocol_Client_newUrl(client, url);
	if (client->type == PROTOCOL_TYPE_FILE)
	{
		if (!strncmp(url + urlLen - 3, "gmi", 3) ||
				!strncmp(url + urlLen - 6, "gemini", 6))
		{
			Parser_setType(parser, PARSER_TYPE_GEMINI);
			printf("File type: gemini\n");
		}
		else if (!strncmp(url + urlLen - 6, "gopher", 6))
		{
			Parser_setType(parser, PARSER_TYPE_GOPHER);
			printf("File type: gopher\n");
		}
		else
		{
			fprintf(stderr, "Non supported file format\n");
			*client = tmpClient;
			return -1;
		}
	}
	else
	{
		if (!strcmp(client->host.scheme, "gemini"))
		{
			if (!strncmp(url + urlLen - 4, ".txt", 4))
			{
				printf("Set type text\n");
				Parser_setType(parser, PARSER_TYPE_TEXT);
			}
			else
			{
				Parser_setType(parser, PARSER_TYPE_GEMINI);
			}
		}
		else if (!strcmp(client->host.scheme, "gopher"))
		{
			char type = client->host.resource[1];
			switch (type)
			{
			case '0':
				Parser_setType(parser, PARSER_TYPE_TEXT);
				break;
			case '1':
			case '\0':
				Parser_setType(parser, PARSER_TYPE_GOPHER);
				break;
			default:
				Parser_setType(parser, PARSER_TYPE_UNKNOWN);
				fprintf(stderr, "ERROR: Unknown type!\n");
				break;
			}
		}
		else if (!strcmp(client->host.scheme, "http") ||
				!strcmp(client->host.scheme, "https"))
		{
			// Open in a different browser
			const char *const openProg = configGlobal.str[GEMCX_CONFIG_STR_HTTP_OPEN];
			const char *const openUrl = protocol_Client_constructUrl(client);
			if (fork() == 0)
			{
				execlp(openProg, openProg, openUrl, NULL);
				printf("Opened '%s' in a different browser\n",
						openUrl);
			}
			*client = tmpClient;
			return 1;
		}
		else
		{
			fprintf(stderr, "Non supported scheme\n");
			*client = tmpClient;
			return -2;
		}
	}

	if (parserHasInit)
	{
		Parser_deinit(parser);
	}
	Parser_init(parser);
	parserHasInit = true;

	if (client->type == PROTOCOL_TYPE_FILE)
	{
		Parser_parse(parser, url + strlen("file://"));
	}
	else
	{
		FILE *reqFp = tmpfile();
		protocol_Client_printInfo(client);
		const int32_t error = protocol_Client_request(client, reqFp);
		if (error)
		{
			fprintf(stderr, "REQUEST ERROR: %s\n",
					protocol_Client_getErrorStr(client, error));
			fclose(reqFp);
			reqFp = NULL;
			return -3;
		}

		if (parser->type == PARSER_TYPE_GEMINI)
		{
			char line[1024] = { 0 };
			rewind(reqFp);

			// TEMP: Skip header
			fgets(line, sizeof(line), reqFp);
			//printf("response header: %s\n", line);

			struct protocol_Header header = { 0 };
			protocol_Header_getLine(&header, line);
			protocol_Header_print(&header);
			const char *statusStr = protocol_Header_getStatusCodeStr(&header);
			const int32_t statusCode = protocol_Header_getStatusCode(&header);
			printf("connectUrl: Status string: %s\n", statusStr);
			switch (statusCode)
			{
			case P_GEMINI_HEADER_STATUSCODE_SUCCESS:
				printf("connectUrl: Success\n");
				break;
			case P_GEMINI_HEADER_STATUSCODE_INPUT:
				// TODO
				printf("connectUrl: input (not implemented)\n");
				break;
			case P_GEMINI_HEADER_STATUSCODE_REDIRECT:
				printf("connectUrl: Redirect (not implemented)\n");
#if 0
				gemcx_xcb_ConnectUrl_connect(client, parser,
						header.header.gemini.meta);
#endif
				break;
			case P_GEMINI_HEADER_STATUSCODE_CLIENT_CERTIFICATE_REQUIRED:
			case P_GEMINI_HEADER_STATUSCODE_TEMPORARY_FAILURE:
			case P_GEMINI_HEADER_STATUSCODE_PERMANENT_FAILURE:
				printf("connectUrl: Failure\n");
				break;
			}
		}
		Parser_parseFp(parser, reqFp, false);

		fclose(reqFp);
		reqFp = NULL;
	}

	return 0;
}

void
gemcx_xcb_ConnectUrl_connectRender(struct gemcx_xcb_ConnectUrl *connectUrl,
		char *urlStr,
		const bool pushUrl)
{
	if (pushUrl)
	{
		protocol_HistoryStack_push(connectUrl->historyStack, urlStr);
	}

	const int32_t connVal = gemcx_xcb_ConnectUrl_connect(connectUrl->client, connectUrl->parser, urlStr);
	if (connVal != 0)
	{
		strcpy(connectUrl->urlInput->str, protocol_HistoryStack_pop(connectUrl->historyStack));
		return;
	}

	strcpy(urlStr, protocol_Client_constructUrl(connectUrl->client));
	ui_xcb_TextInput_textReRender(connectUrl->urlInput);
	render_Xcb_itemsInit(connectUrl->pxcb, connectUrl->parser);

	*connectUrl->mainAreaYoffset = 0;
	render_Xcb_offset(connectUrl->pxcb, 0, -*connectUrl->mainAreaYoffset);
	*connectUrl->mainAreaYMax = render_Xcb_render(connectUrl->pxcb,
			connectUrl->parser);
	ui_xcb_Pixmap_render(connectUrl->mainArea, 0, 0);
	ui_xcb_Pixmap_render(connectUrl->doubleBuffer, 0, 0);

	protocol_HistoryStack_print(connectUrl->historyStack);	// TEMP
}

