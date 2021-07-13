// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_close_response.h>
#include <sml/sml_log.h>

// sml_close_response;

sml_close_response *sml_close_response_init(void)
{
	sml_close_response *msg = (sml_close_response *)malloc(sizeof(sml_close_response));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_close_response));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_close_response));
	
	return msg;
}

sml_close_response *sml_close_response_parse(sml_buffer *buf)
{
	sml_close_response *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_close_response_init();
	if (!msg)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 1)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tCLOSE RESPONSE\r\n");
	}

	msg->global_signature = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_close_response_free(msg);
	return NULL;
}

int sml_close_response_write(sml_close_response *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 1);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->global_signature, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_close_response_free(sml_close_response *msg)
{
	if (msg)
    {
		sml_octet_string_free(msg->global_signature);

		free(msg);
	}
}
