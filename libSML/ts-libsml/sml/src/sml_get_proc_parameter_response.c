// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_get_proc_parameter_response.h>
#include <sml/sml_log.h>

// sml_get_proc_parameter_response;

sml_get_proc_parameter_response *sml_get_proc_parameter_response_init(void)
{
	sml_get_proc_parameter_response *msg = (sml_get_proc_parameter_response *)malloc(sizeof(sml_get_proc_parameter_response));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_proc_parameter_response));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_get_proc_parameter_response));
	
	return msg;
}

sml_get_proc_parameter_response *sml_get_proc_parameter_response_parse(sml_buffer *buf)
{
	sml_get_proc_parameter_response *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_get_proc_parameter_response_init();
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

	if (sml_buf_get_next_length(buf) != 3)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tGET PROC PARAMETER RESPONSE\r\n");
	}

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->parameter_tree_path = sml_tree_path_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->parameter_tree = sml_tree_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_get_proc_parameter_response_free(msg);
	return NULL;
}

int sml_get_proc_parameter_response_write(sml_get_proc_parameter_response *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_tree_path_write(msg->parameter_tree_path, buf);
	if (rv != SML_OK) return rv;

	rv = sml_tree_write(msg->parameter_tree, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_get_proc_parameter_response_free(sml_get_proc_parameter_response *msg)
{
	if (msg)
    {
		sml_octet_string_free(msg->server_id);
		sml_tree_path_free(msg->parameter_tree_path);
		sml_tree_free(msg->parameter_tree);

		free(msg);
	}
}
