// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_get_profile_pack_request.h>
#include <sml/sml_tree.h>
#include <sml/sml_boolean.h>
#include <sml/sml_time.h>
#include <sml/sml_log.h>

// sml_get_profile_pack_request;

sml_get_profile_pack_request *sml_get_profile_pack_request_init(void)
{
	sml_get_profile_pack_request *msg = (sml_get_profile_pack_request *)malloc(sizeof(sml_get_profile_pack_request));
	if (!msg)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_profile_pack_request));
		return NULL;
	}
	memset(msg, 0, sizeof(sml_get_profile_pack_request));
	
	return msg;
}

sml_get_profile_pack_request *sml_get_profile_pack_request_parse(sml_buffer *buf)
{
	sml_get_profile_pack_request *msg = NULL;

	if (!buf)
	{
		return NULL;
	}

	msg = sml_get_profile_pack_request_init();
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

	if (sml_buf_get_next_length(buf) != 9)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tGET PROFILE PACK REQUEST\r\n");
	}

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->username = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->password = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->with_rawdata = sml_boolean_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->begin_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->end_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->parameter_tree_path = sml_tree_path_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	if (!sml_buf_optional_is_skipped(buf))
    {
        int i, len = sml_buf_get_next_length(buf);
		sml_obj_req_entry_list *last = 0, *n = 0;
        
        if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
        {
			buf->error = SML_TYPE_LIST_EXPECTED;
			goto error;
		}

		for (i = len; i > 0; i--)
        {
			n = (sml_obj_req_entry_list *)malloc(sizeof(sml_obj_req_entry_list));
			if (!n)
            {
				trace("ERROR: malloc %ld bytes\n", sizeof(sml_obj_req_entry_list));
				buf->error = SML_MALLOC_ERROR;
				return NULL;
			}

			memset(n, 0, sizeof(sml_obj_req_entry_list));
			n->object_list_entry = sml_obj_req_entry_parse(buf);
			if (sml_buf_has_errors(buf)) goto error;

			if (msg->object_list == 0)
            {
				msg->object_list = n;
				last = msg->object_list;
			}
			else
            {
				last->next = n;
				last = n;
			}
		}
	}

	msg->das_details = sml_tree_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_get_profile_pack_request_free(msg);
	return NULL;
}

int sml_get_profile_pack_request_write(sml_get_profile_pack_request *msg, sml_buffer *buf)
{
	int rv;

	if (!msg || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 9);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->username, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(msg->password, buf);
	if (rv != SML_OK) return rv;

	rv = sml_boolean_write(msg->with_rawdata, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(msg->begin_time, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(msg->end_time, buf);
	if (rv != SML_OK) return rv;

	rv = sml_tree_path_write(msg->parameter_tree_path, buf);
	if (rv != SML_OK) return rv;

	if (msg->object_list)
    {
		int len = 1;
		sml_obj_req_entry_list *l = msg->object_list;
		for (l = msg->object_list; l->next; l = l->next)
        {
			len++;
		}
		rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, len);
		if (rv != SML_OK) return rv;

		for (l = msg->object_list; l->next; l = l->next)
        {
			rv = sml_obj_req_entry_write(l->object_list_entry, buf);
			if (rv != SML_OK) return rv;
		}
		rv = sml_obj_req_entry_write(l->object_list_entry, buf);
		if (rv != SML_OK) return rv;
	}
	else
    {
		rv = sml_buf_optional_write(buf);
		if (rv != SML_OK) return rv;
	}

	rv = sml_tree_write(msg->das_details, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_get_profile_pack_request_free(sml_get_profile_pack_request *msg)
{
	 if (msg)
     {
		sml_octet_string_free(msg->server_id);
		sml_octet_string_free(msg->username);
		sml_octet_string_free(msg->password);
		sml_boolean_free(msg->with_rawdata);
		sml_time_free(msg->begin_time);
		sml_time_free(msg->end_time);
		sml_tree_path_free(msg->parameter_tree_path);

		if (msg->object_list)
        {
			sml_obj_req_entry_list *n = 0, *d = msg->object_list;
			do
            {
				n = d->next;
				sml_obj_req_entry_free(d->object_list_entry);
				free(d);
				d = n;
			} while (d);
		}
		
		sml_tree_free(msg->das_details);
		free(msg);
	}
}
