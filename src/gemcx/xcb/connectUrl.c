#include "gemcx/xcb/connectUrl.h"

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

		if (!strncmp(url + urlLen - 3, "gmi", 3))
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

	int32_t retVal = 0;

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
			retVal = -2;
			goto connUrlExit;
		}

		if (parser->type == PROTOCOL_TYPE_GEMINI)
		{
			char line[1024] = { 0 };
			rewind(reqFp);

			// TEMP: Skip header
			fgets(line, sizeof(line), reqFp);
		}
		protocol_Parser_parseFp(parser, reqFp, false);
		fclose(reqFp);
		reqFp = NULL;
	}

connUrlExit:

	return retVal;
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

