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
#include <sml/sml_get_profile_pack_response.h>
#include <sml/sml_log.h>

// sml_get_profile_pack_response;

sml_get_profile_pack_response *sml_get_profile_pack_response_init() {
	sml_get_profile_pack_response *msg = (sml_get_profile_pack_response *)malloc(sizeof(sml_get_profile_pack_response));
	if (msg) {
		memset(msg, 0, sizeof(sml_get_profile_pack_response));
	}
	return msg;
}

sml_get_profile_pack_response *sml_get_profile_pack_response_parse(sml_buffer *buf) {
	sml_get_profile_pack_response *msg = sml_get_profile_pack_response_init();
	if (!msg) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_get_profile_pack_response));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 8) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tGET PROFILE PACK RESPONSE\r\n");
	}

	msg->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->act_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->reg_period = sml_u32_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->parameter_tree_path = sml_tree_path_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->header_list = sml_sequence_parse(buf, (void *(*)(sml_buffer *)) &sml_prof_obj_header_entry_parse, (void (*)(void *)) &sml_prof_obj_header_entry_free);
	if (sml_buf_has_errors(buf)) goto error;

	msg->period_list = sml_sequence_parse(buf, (void *(*)(sml_buffer *)) &sml_prof_obj_period_entry_parse, (void (*)(void *)) &sml_prof_obj_period_entry_free);
	if (sml_buf_has_errors(buf)) goto error;

	msg->rawdata = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->profile_signature = sml_signature_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return msg;

error:
	sml_get_profile_pack_response_free(msg);
	return 0;
}

void sml_get_profile_pack_response_write(sml_get_profile_pack_response *msg, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 8);

	sml_octet_string_write(msg->server_id, buf);
	sml_time_write(msg->act_time, buf);
	sml_u32_write(msg->reg_period, buf);
	sml_tree_path_write(msg->parameter_tree_path, buf);
	sml_sequence_write(msg->header_list, buf, (void (*)(void *, sml_buffer *)) &sml_prof_obj_header_entry_write);
	sml_sequence_write(msg->period_list, buf, (void (*)(void *, sml_buffer *)) &sml_prof_obj_period_entry_write);
	sml_octet_string_write(msg->rawdata, buf);
	sml_signature_write(msg->profile_signature, buf);
}

void sml_get_profile_pack_response_free(sml_get_profile_pack_response *msg) {
	if (msg) {
		sml_octet_string_free(msg->server_id);
		sml_time_free(msg->act_time);
		sml_number_free(msg->reg_period);
		sml_tree_path_free(msg->parameter_tree_path);
		sml_sequence_free(msg->header_list);
		sml_sequence_free(msg->period_list);
		sml_octet_string_free(msg->rawdata);
		sml_signature_free(msg->profile_signature);

		free(msg);
	}
}


// sml_prof_obj_header_entry;

sml_prof_obj_header_entry *sml_prof_obj_header_entry_init() {
	sml_prof_obj_header_entry *entry = (sml_prof_obj_header_entry *)malloc(sizeof(sml_prof_obj_header_entry));
	if (entry) {
		memset(entry, 0, sizeof(sml_prof_obj_header_entry));
	}
	return entry;
}

sml_prof_obj_header_entry *sml_prof_obj_header_entry_parse(sml_buffer *buf) {
	sml_prof_obj_header_entry *entry = sml_prof_obj_header_entry_init();
	if (!entry) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_prof_obj_header_entry));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 3) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tPROF OBJ HEADER ENTRY\r\n");
	}

	entry->obj_name = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	entry->unit = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	entry->scaler = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return entry;
error:
	sml_prof_obj_header_entry_free(entry);
	return 0;
}

void sml_prof_obj_header_entry_write(sml_prof_obj_header_entry *entry, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);

	sml_octet_string_write(entry->obj_name, buf);
	sml_unit_write(entry->unit, buf);
	sml_i8_write(entry->scaler, buf);
}

void sml_prof_obj_header_entry_free(sml_prof_obj_header_entry *entry) {
	if (entry) {
		sml_octet_string_free(entry->obj_name);
		sml_unit_free(entry->unit);
		sml_number_free(entry->scaler);

		free(entry);
	}
}


// sml_prof_obj_period_entry;

sml_prof_obj_period_entry *sml_prof_obj_period_entry_init() {
	sml_prof_obj_period_entry *entry = (sml_prof_obj_period_entry *)malloc(sizeof(sml_prof_obj_period_entry));
	if (entry) {
		memset(entry, 0, sizeof(sml_prof_obj_period_entry));
	}
	return entry;
}

sml_prof_obj_period_entry *sml_prof_obj_period_entry_parse(sml_buffer *buf) {
	sml_prof_obj_period_entry *entry = sml_prof_obj_period_entry_init();
	if (!entry) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_prof_obj_period_entry));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 4) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tPROF OBJ PERIOD ENTRY\r\n");
	}

	entry->val_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	entry->status = sml_u64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	entry->value_list = sml_sequence_parse(buf, (void *(*)(sml_buffer *)) &sml_value_entry_parse, (void (*)(void *)) &sml_value_entry_free);
	if (sml_buf_has_errors(buf)) goto error;
	entry->period_signature = sml_signature_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return entry;

error:
	sml_prof_obj_period_entry_free(entry);
	return 0;
}

void sml_prof_obj_period_entry_write(sml_prof_obj_period_entry *entry, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 4);
	sml_time_write(entry->val_time, buf);
	sml_u64_write(entry->status, buf);
	sml_sequence_write(entry->value_list, buf, (void (*)(void *, sml_buffer *)) &sml_value_entry_write);
	sml_signature_write(entry->period_signature, buf);
}

void sml_prof_obj_period_entry_free(sml_prof_obj_period_entry *entry) {
	if (entry) {
		sml_time_free(entry->val_time);
		sml_number_free(entry->status);
		sml_sequence_free(entry->value_list);
		sml_signature_free(entry->period_signature);

		free(entry);
	}
}


// sml_value_entry;

sml_value_entry *sml_value_entry_init() {
	sml_value_entry *entry = (sml_value_entry *)malloc(sizeof(sml_value_entry));
	if (entry) {
		memset(entry, 0, sizeof(sml_value_entry));
	}
	return entry;
}

sml_value_entry *sml_value_entry_parse(sml_buffer *buf) {
	sml_value_entry *entry = sml_value_entry_init();
	if (!entry) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_value_entry));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 2) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tVALUE ENTRY\r\n");
	}

	entry->value = sml_value_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	entry->value_signature = sml_signature_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return entry;

error:
	sml_value_entry_free(entry);
	return 0;
}

void sml_value_entry_write(sml_value_entry *entry, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	sml_value_write(entry->value, buf);
	sml_signature_write(entry->value_signature, buf);
}

void sml_value_entry_free(sml_value_entry *entry) {
	if (entry) {
		sml_value_free(entry->value);
		sml_signature_free(entry->value_signature);

		free(entry);
	}
}

