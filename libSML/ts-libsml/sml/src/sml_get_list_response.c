// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_get_list_response.h>
#include <sml/sml_log.h>

// sml_get_list_response;

sml_get_list_response *sml_get_list_response_init(void)
{
	sml_get_list_response *msg = (sml_get_list_response *)malloc(sizeof(sml_get_list_response));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_list_response));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_get_list_response));
	
	return msg;
}

sml_get_list_response *sml_get_list_response_parse(sml_buffer *buf)
{
	sml_get_list_response *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_get_list_response_init();
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
		trace("\t\tGET LIST RESPONSE\r\n");
	}

	msg->client_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->list_name = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->act_sensor_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->val_list = sml_list_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->list_signature = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->act_gateway_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_get_list_response_free(msg);
	return NULL;
}

int sml_get_list_response_write(sml_get_list_response *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 7);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->client_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->list_name, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(msg->act_sensor_time, buf);
	if (rv != SML_OK) return rv;

	rv = sml_list_write(msg->val_list, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->list_signature, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(msg->act_gateway_time, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_get_list_response_free(sml_get_list_response *msg)
{
	if (msg)
    {
		sml_octet_string_free(msg->client_id);
		sml_octet_string_free(msg->server_id);
		sml_octet_string_free(msg->list_name);
		sml_time_free(msg->act_sensor_time);
		sml_list_free(msg->val_list);
		sml_octet_string_free(msg->list_signature);
		sml_time_free(msg->act_gateway_time);

		free(msg);
	}
}
