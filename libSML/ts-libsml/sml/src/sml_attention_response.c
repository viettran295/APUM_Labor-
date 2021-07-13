// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_attention_response.h>
#include <sml/sml_tree.h>
#include <sml/sml_log.h>

// sml_attention_response;

sml_attention_response *sml_attention_response_init(void)
{
	sml_attention_response *msg = (sml_attention_response *)malloc(sizeof(sml_attention_response));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_attention_response));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_attention_response));
	
	return msg;
}

sml_attention_response *sml_attention_response_parse(sml_buffer *buf)
{
	sml_attention_response *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_attention_response_init();
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

	if (sml_buf_get_next_length(buf) != 4)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tATTENTION RESPONSE\r\n");
	}

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->attention_number = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->attention_message = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->attention_details = sml_tree_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_attention_response_free(msg);
	return NULL;
}

int sml_attention_response_write(sml_attention_response *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 4);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->attention_number, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->attention_message, buf);
	if (rv != SML_OK) return rv;

	rv = sml_tree_write(msg->attention_details, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_attention_response_free(sml_attention_response *msg)
{
	if (msg)
    {
		sml_octet_string_free(msg->server_id);
		sml_octet_string_free(msg->attention_number);
		sml_octet_string_free(msg->attention_message);
		sml_tree_free(msg->attention_details);

		free(msg);
	}
}
