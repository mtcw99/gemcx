#include "protocol/gemini/header.h"

#include <stdlib.h>
#include <string.h>

static const char *statusCodeStr[P_GEMINI_HEADER_STATUSCODE__TOTAL] = {
	[P_GEMINI_HEADER_STATUSCODE_INPUT] = "INPUT",
	[P_GEMINI_HEADER_STATUSCODE_SUCCESS] = "SUCCESS",
	[P_GEMINI_HEADER_STATUSCODE_REDIRECT] = "REDIRECT",
	[P_GEMINI_HEADER_STATUSCODE_TEMPORARY_FAILURE] = "TEMPORARY_FAILURE",
	[P_GEMINI_HEADER_STATUSCODE_PERMANENT_FAILURE] = "PERMANENT_FAILURE",
	[P_GEMINI_HEADER_STATUSCODE_CLIENT_CERTIFICATE_REQUIRED] = "CLIENT_CERTIFICATE_REQUIRED"
};

void
p_gemini_Header_get(struct p_gemini_Header *header,
		FILE *fp)
{
	rewind(fp);
	char headerStr[1036] = { 0 };
	if (fgets(headerStr, 1027, fp) != NULL)
	{
		const uint32_t headerLen = strlen(headerStr);
		strncpy(header->statusCode, headerStr, 2);
		strncpy(header->meta, headerStr+3, headerLen-6);
	}
}

void
p_gemini_Header_print(const struct p_gemini_Header *header)
{
	printf("Header:\n"
			"\tStatus Code: %s (%s)\n"
			"\tMeta: %s\n\n",
			header->statusCode,
			p_gemini_Header_getStatusCodeStr(header),
			header->meta);
}

bool
p_gemini_Header_isGemini(const struct p_gemini_Header *header)
{
	return (strncmp(header->meta, "text/gemin", 10) == 0);
}

const char *
p_gemini_Header_getStatusCodeStr(const struct p_gemini_Header *header)
{
	const int32_t firstDigit = header->statusCode[0] - '0';
	return (firstDigit > 0) ? statusCodeStr[firstDigit] : "Status code INVALID";
}

