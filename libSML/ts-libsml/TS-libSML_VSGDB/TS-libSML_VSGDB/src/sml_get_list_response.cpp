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
#include <sml/sml_get_list_response.h>
#include <sml/sml_log.h>

// sml_get_list_response;

sml_get_list_response *sml_get_list_response_init() {
	sml_get_list_response *msg = (sml_get_list_response *)malloc(sizeof(sml_get_list_response));
	if (msg) {
		memset(msg, 0, sizeof(sml_get_list_response));
	}
	return msg;
}

sml_get_list_response *sml_get_list_response_parse(sml_buffer *buf) {
	sml_get_list_response *msg = sml_get_list_response_init();
	if (!msg) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_list_response));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 7) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
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
	return 0;
}

void sml_get_list_response_write(sml_get_list_response *msg, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 7);

	sml_octet_string_write(msg->client_id, buf);
	sml_octet_string_write(msg->server_id, buf);
	sml_octet_string_write(msg->list_name, buf);
	sml_time_write(msg->act_sensor_time, buf);
	sml_list_write(msg->val_list, buf);
	sml_octet_string_write(msg->list_signature, buf);
	sml_time_write(msg->act_gateway_time, buf);
}

void sml_get_list_response_free(sml_get_list_response *msg) {
	if (msg) {
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

