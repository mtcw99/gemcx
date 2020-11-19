#ifndef GEMCX_XCB_CONNECTURL_H
#define GEMCX_XCB_CONNECTURL_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/client.h"
#include "protocol/parser.h"
#include "protocol/xcb.h"
#include "protocol/historyStack.h"

#include "ui/xcb/textInput.h"
#include "ui/xcb/pixmap.h"

struct gemcx_xcb_ConnectUrl
{
	struct protocol_Client *client;
	struct protocol_Parser *parser;
	struct protocol_Xcb *pxcb;
	int32_t *mainAreaYoffset;
	struct ui_xcb_TextInput *urlInput;
	uint32_t *mainAreaYMax;
	struct ui_xcb_Pixmap *mainArea;
	struct ui_xcb_Pixmap *doubleBuffer;
	struct protocol_HistoryStack *historyStack;
};

int32_t gemcx_xcb_ConnectUrl_connect(struct protocol_Client *const restrict client,
		struct protocol_Parser *const restrict parser,
		const char *const restrict url);

void gemcx_xcb_ConnectUrl_connectRender(struct gemcx_xcb_ConnectUrl *connectUrl,
		char *urlStr,
		const bool pushUrl);

#endif // GEMCX_XCB_CONNECTURL_H

