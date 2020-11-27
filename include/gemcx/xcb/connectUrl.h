#ifndef GEMCX_XCB_CONNECTURL_H
#define GEMCX_XCB_CONNECTURL_H

#include <stdint.h>
#include <stdbool.h>

#include "protocol/client.h"
#include "protocol/historyStack.h"

#include "parser/generic.h"
#include "render/xcb.h"

#include "ui/xcb/textInput.h"
#include "ui/xcb/pixmap.h"

struct gemcx_xcb_ConnectUrl
{
	struct protocol_Client *client;
	struct Parser *parser;
	struct render_Xcb *pxcb;
	int32_t *mainAreaYoffset;
	struct ui_xcb_TextInput *urlInput;
	uint32_t *mainAreaYMax;
	struct ui_xcb_Pixmap *mainArea;
	struct ui_xcb_Pixmap *doubleBuffer;
	struct protocol_HistoryStack *historyStack;
};

int32_t gemcx_xcb_ConnectUrl_connect(struct protocol_Client *const restrict client,
		struct Parser *const restrict parser,
		const char *const restrict url);

void gemcx_xcb_ConnectUrl_connectRender(struct gemcx_xcb_ConnectUrl *connectUrl,
		char *urlStr,
		const bool pushUrl);

#endif // GEMCX_XCB_CONNECTURL_H

