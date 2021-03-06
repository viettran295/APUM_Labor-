// Copyright 2011 Juri Glass, Mathias Runge, Nadim El Sayed
// DAI-Labor, TU-Berlin
//
// This file is part of libSML.
//
// libSML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libSML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libSML.  If not, see <http://www.gnu.org/licenses/>.


#include <stdio.h>
#include <sml/sml_get_profile_pack_request.h>
#include <sml/sml_tree.h>
#include <sml/sml_boolean.h>
#include <sml/sml_time.h>
#include <sml/sml_log.h>

// sml_get_profile_pack_request;

sml_get_profile_pack_request *sml_get_profile_pack_request_init() {
	sml_get_profile_pack_request *msg = (sml_get_profile_pack_request *)malloc(sizeof(sml_get_profile_pack_request));
	if (msg) {
		memset(msg, 0, sizeof(sml_get_profile_pack_request));
	}
	return msg;
}

sml_get_profile_pack_request *sml_get_profile_pack_request_parse(sml_buffer *buf) {
	sml_get_profile_pack_request *msg = sml_get_profile_pack_request_init();
	if (!msg) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_profile_pack_request));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 9) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
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

	if (!sml_buf_optional_is_skipped(buf)) {
		if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
			buf->error = SML_TYPE_LIST_EXPECTED;
			goto error;
		}
		int i, len = sml_buf_get_next_length(buf);
		sml_obj_req_entry_list *last = 0, *n = 0;
		for (i = len; i > 0; i--) {
			n = (sml_obj_req_entry_list *)malloc(sizeof(sml_obj_req_entry_list));
			if (!n) {
				trace("ERROR: malloc %ld bytes\n", sizeof(sml_obj_req_entry_list));
				buf->error = SML_MALLOC_ERROR;
				return NULL;
			}

			memset(n, 0, sizeof(sml_obj_req_entry_list));
			n->object_list_entry = sml_obj_req_entry_parse(buf);
			if (sml_buf_has_errors(buf)) goto error;

			if (msg->object_list == 0) {
				msg->object_list = n;
				last = msg->object_list;
			}
			else {
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
	return 0;
}

void sml_get_profile_pack_request_write(sml_get_profile_pack_request *msg, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 9);

	sml_octet_string_write(msg->server_id, buf);
	sml_octet_string_write(msg->username, buf);
	sml_octet_string_write(msg->password, buf);
	sml_boolean_write(msg->with_rawdata, buf);
	sml_time_write(msg->begin_time, buf);
	sml_time_write(msg->end_time, buf);
	sml_tree_path_write(msg->parameter_tree_path, buf);

	if (msg->object_list) {
		int len = 1;
		sml_obj_req_entry_list *l = msg->object_list;
		for (l = msg->object_list; l->next; l = l->next) {
			len++;
		}
		sml_buf_set_type_and_length(buf, SML_TYPE_LIST, len);
		for (l = msg->object_list; l->next; l = l->next) {
			sml_obj_req_entry_write(l->object_list_entry, buf);
		}
		sml_obj_req_entry_write(l->object_list_entry, buf);
	}
	else {
		sml_buf_optional_write(buf);
	}

	sml_tree_write(msg->das_details, buf);
}

void sml_get_profile_pack_request_free(sml_get_profile_pack_request *msg) {
	 if (msg) {
		sml_octet_string_free(msg->server_id);
		sml_octet_string_free(msg->username);
		sml_octet_string_free(msg->password);
		sml_boolean_free(msg->with_rawdata);
		sml_time_free(msg->begin_time);
		sml_time_free(msg->end_time);
		sml_tree_path_free(msg->parameter_tree_path);

		if (msg->object_list) {
			sml_obj_req_entry_list *n = 0, *d = msg->object_list;
			do {
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

