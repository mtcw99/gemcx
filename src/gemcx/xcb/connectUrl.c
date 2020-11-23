#include "gemcx/xcb/connectUrl.h"

#include "protocol/header.h"

int32_t
gemcx_xcb_ConnectUrl_connect(struct protocol_Client *const restrict client,
		struct protocol_Parser *const restrict parser,
		const char *const restrict url)
{
	static bool parserHasInit = false;
	printf("Connecting: '%s'\n", url);

	protocol_Client_newUrl(client, url);
	if (client->type == PROTOCOL_TYPE_FILE)
	{
		const uint32_t urlLen = strlen(url);

		if (!strncmp(url + urlLen - 3, "gmi", 3) ||
				!strncmp(url + urlLen - 6, "gemini", 6))
		{
			protocol_Parser_setType(parser, PROTOCOL_TYPE_GEMINI);
			printf("File type: gemini\n");
		}
		else if (!strncmp(url + urlLen - 6, "gopher", 6))
		{
			protocol_Parser_setType(parser, PROTOCOL_TYPE_GOPHER);
			printf("File type: gopher\n");
		}
		else
		{
			fprintf(stderr, "Non supported file format\n");
			return -1;
		}
	}
	else
	{
		protocol_Parser_setType(parser, client->type);
	}

	if (parserHasInit)
	{
		protocol_Parser_deinit(parser);
	}
	protocol_Parser_init(parser);
	parserHasInit = true;

	if (client->type == PROTOCOL_TYPE_FILE)
	{
		protocol_Parser_parse(parser, url + strlen("file://"));
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
			return -2;
		}

		if (parser->type == PROTOCOL_TYPE_GEMINI)
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
				printf("connectUrl: Redirect\n");
				gemcx_xcb_ConnectUrl_connect(client, parser,
						header.header.gemini.meta);
				break;
			case P_GEMINI_HEADER_STATUSCODE_CLIENT_CERTIFICATE_REQUIRED:
			case P_GEMINI_HEADER_STATUSCODE_TEMPORARY_FAILURE:
			case P_GEMINI_HEADER_STATUSCODE_PERMANENT_FAILURE:
				printf("connectUrl: Failure\n");
				break;
			}
		}
		protocol_Parser_parseFp(parser, reqFp, false);

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
	gemcx_xcb_ConnectUrl_connect(connectUrl->client, connectUrl->parser, urlStr);
	strcpy(urlStr, protocol_Client_constructUrl(connectUrl->client));
	ui_xcb_TextInput_textReRender(connectUrl->urlInput);
	protocol_Xcb_itemsInit(connectUrl->pxcb, connectUrl->parser);

	*connectUrl->mainAreaYoffset = 0;
	protocol_Xcb_offset(connectUrl->pxcb, 0, -*connectUrl->mainAreaYoffset);
	*connectUrl->mainAreaYMax = protocol_Xcb_render(connectUrl->pxcb,
			connectUrl->parser);
	ui_xcb_Pixmap_render(connectUrl->mainArea, 0, 0);
	ui_xcb_Pixmap_render(connectUrl->doubleBuffer, 0, 0);

	if (pushUrl)
	{
		protocol_HistoryStack_push(connectUrl->historyStack, urlStr);
	}
	protocol_HistoryStack_print(connectUrl->historyStack);	// TEMP
}

