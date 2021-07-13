// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_open_request.h>
#include <sml/sml_number.h>
#include <sml/sml_log.h>

// sml_open_request;

sml_open_request *sml_open_request_init(void)
{
	sml_open_request *msg = (sml_open_request *)malloc(sizeof(sml_open_request));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_open_request));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_open_request));
	
	return msg;
}

sml_open_request *sml_open_request_parse(sml_buffer *buf)
{
	sml_open_request *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_open_request_init();
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

	if (sml_buf_get_next_length(buf) != 7)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tOPEN REQUEST\r\n");
	}

	msg->codepage = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->client_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->req_file_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->username = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->password = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->sml_version = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_open_request_free(msg);
	return NULL;
}

int sml_open_request_write(sml_open_request *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 7);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->codepage, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->client_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->req_file_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->username,buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->password,buf);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(msg->sml_version, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_open_request_free(sml_open_request *msg)
{
	if (msg)
    {
		sml_octet_string_free(msg->codepage);
		sml_octet_string_free(msg->client_id);
		sml_octet_string_free(msg->req_file_id);
		sml_octet_string_free(msg->server_id);
		sml_octet_string_free(msg->username);
		sml_octet_string_free(msg->password);
		sml_number_free(msg->sml_version);
		
		free(msg);
	}
}
