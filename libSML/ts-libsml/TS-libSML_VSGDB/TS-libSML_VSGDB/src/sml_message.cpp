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

#ifndef WIN32
	#ifndef USE_HAL_DRIVER
		#include <unistd.h>
	#endif
#endif

#include <sml/sml_message.h>
#include <sml/sml_number.h>
#include <sml/sml_octet_string.h>
#include <sml/sml_shared.h>
#include <sml/sml_list.h>
#include <sml/sml_time.h>
#include <sml/sml_crc16.h>
#include <sml/sml_log.h>

// sml_message;

sml_message *sml_message_init() {
	sml_message *msg = (sml_message *)malloc(sizeof(sml_message));
	if (!msg) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_message));
		return NULL;
	}

	memset(msg, 0, sizeof(sml_message));
	msg->transaction_id = sml_octet_string_generate_uuid();

	return msg;
}

sml_message *sml_message_parse(sml_buffer *buf) {
	int startCursor = buf->cursor;
	sml_message *msg = (sml_message *)malloc(sizeof(sml_message));
	if (!msg) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_message));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	memset(msg, 0, sizeof(sml_message));

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 6) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tMESSAGE\r\n");
	}

	msg->transaction_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->group_id = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->abort_on_error = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	msg->message_body = sml_message_body_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	u16 crc;
	crc = sml_crc16_calculate(buf->buffer + startCursor, buf->cursor - startCursor);

	msg->crc = sml_u16_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	if (crc != *msg->crc) {
		buf->error = SML_MESSAGE_CRC_ERROR;
		goto error;
	}

	if (sml_buf_get_current_byte(buf) == SML_MESSAGE_END) {
		sml_buf_update_bytes_read(buf, 1);
	}
	
	return msg;

error:
	sml_message_free(msg);
	return 0;
}

void sml_message_write(sml_message *msg, sml_buffer *buf) {
	int msg_start = buf->cursor;
	
	if (buf->buffer_len - msg_start < SML_FILE_BUFFER_MESSAGE_LENGTH) {
		//buf->buffer_len += SML_FILE_BUFFER_MESSAGE_LENGTH;
		//buf->buffer = (unsigned char*)realloc(buf->buffer, buf->buffer_len);
		//if (!buf->buffer) {
		//	trace("ERROR: malloc %ld bytes\n", buf->buffer_len);
		//	return;
		//}

		unsigned char *buffer = NULL;
		size_t buffer_len = buf->buffer_len + SML_FILE_BUFFER_MESSAGE_LENGTH;
		buffer = (unsigned char*)malloc(buffer_len);
		if (!buffer) {
			trace("ERROR: malloc %ld bytes\n", buffer_len);
			buf->error = SML_MALLOC_ERROR;
			return;
		}
		if (buf->buffer) {
			memcpy(buffer, buf->buffer, buf->buffer_len);
			free(buf->buffer);
		}
		buf->buffer = buffer;
		buf->buffer_len = buffer_len;
	}

	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 6);
	sml_octet_string_write(msg->transaction_id, buf);
	sml_u8_write(msg->group_id, buf);
	sml_u8_write(msg->abort_on_error, buf);
	sml_message_body_write(msg->message_body, buf);

	if (msg->crc) {
		sml_number_free(msg->crc);
	}
	msg->crc = sml_u16_init(sml_crc16_calculate(&(buf->buffer[msg_start]), buf->cursor - msg_start));
	sml_u16_write(msg->crc, buf);

	// end of message
	buf->buffer[buf->cursor] = 0x0;
	buf->cursor++;
}

void sml_message_free(sml_message *msg) {
	if (msg) {
		sml_octet_string_free(msg->transaction_id);
		sml_number_free(msg->group_id);
		sml_number_free(msg->abort_on_error);
		sml_message_body_free(msg->message_body);
		sml_number_free(msg->crc);
		free(msg);
	}
}

// sml_message_body;

sml_message_body *sml_message_body_init(u32 tag, void *data) {
	sml_message_body *message_body = (sml_message_body *)malloc(sizeof(sml_message_body));
	if (!message_body) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_message_body));
		return NULL;
	}

	memset(message_body, 0, sizeof(sml_message_body));
	message_body->tag = sml_u32_init(tag);
	message_body->d.data = data;

	return message_body;
}

sml_message_body *sml_message_body_parse(sml_buffer *buf) {
	sml_message_body *msg_body = (sml_message_body *)malloc(sizeof(sml_message_body));
	if (!msg_body) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_message_body));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	memset(msg_body, 0, sizeof(sml_message_body));

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 2) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	msg_body->tag = sml_u32_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	if (sml_debug_output(buf)) {
		trace("\t\tMESSAGE BODY\t%i\r\n", *(msg_body->tag));
	}

	switch (*(msg_body->tag)) {
		case SML_MESSAGE_OPEN_REQUEST:
			msg_body->d.open_request = sml_open_request_parse(buf);
			break;
		case SML_MESSAGE_OPEN_RESPONSE:
			msg_body->d.open_response = sml_open_response_parse(buf);
			break;
		case SML_MESSAGE_CLOSE_REQUEST:
			msg_body->d.close_request = sml_close_request_parse(buf);
			break;
		case SML_MESSAGE_CLOSE_RESPONSE:
			msg_body->d.close_response = sml_close_response_parse(buf);
			break;
		case SML_MESSAGE_GET_PROFILE_PACK_REQUEST:
			msg_body->d.get_profile_pack_request = sml_get_profile_pack_request_parse(buf);
			break;
		case SML_MESSAGE_GET_PROFILE_PACK_RESPONSE:
			msg_body->d.get_profile_pack_response = sml_get_profile_pack_response_parse(buf);
			break;
		case SML_MESSAGE_GET_PROFILE_LIST_REQUEST:
			msg_body->d.get_profile_list_request = sml_get_profile_list_request_parse(buf);
			break;
		case SML_MESSAGE_GET_PROFILE_LIST_RESPONSE:
			msg_body->d.get_profile_list_response = sml_get_profile_list_response_parse(buf);
			break;
		case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
			msg_body->d.get_proc_parameter_request = sml_get_proc_parameter_request_parse(buf);
			break;
		case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
			msg_body->d.get_proc_parameter_response = sml_get_proc_parameter_response_parse(buf);
			break;
		case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
			msg_body->d.set_proc_parameter_request = sml_set_proc_parameter_request_parse(buf);
			break;
		case SML_MESSAGE_GET_LIST_REQUEST:
			msg_body->d.get_list_request = sml_get_list_request_parse(buf);
			break;
		case SML_MESSAGE_GET_LIST_RESPONSE:
			msg_body->d.get_list_response = sml_get_list_response_parse(buf);
			break;
		case SML_MESSAGE_ATTENTION_RESPONSE:
			msg_body->d.attention_response = sml_attention_response_parse(buf);
			break;
		default:
			buf->error = SML_MESSAGE_TAG_MISMATCH;
			if (sml_debug_output(buf)) {
				trace("error: message type %04X not yet implemented\n", *(msg_body->tag));
			}
			break;
	}

	return msg_body;

error:
	free(msg_body);
	return 0;
}

void sml_message_body_write(sml_message_body *message_body, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);

	if (buf->flags & SML_FLAGS_WRITE_CHOICE_COMPACT) {
		sml_u16_write(message_body->tag, buf);
	}
	else {
		sml_u32_write(message_body->tag, buf);
	}

	switch (*(message_body->tag)) {
		case SML_MESSAGE_OPEN_REQUEST:
			sml_open_request_write(message_body->d.open_request, buf);
			break;
		case SML_MESSAGE_OPEN_RESPONSE:
			sml_open_response_write(message_body->d.open_response, buf);
			break;
		case SML_MESSAGE_CLOSE_REQUEST:
			sml_close_request_write(message_body->d.close_request, buf);
			break;
		case SML_MESSAGE_CLOSE_RESPONSE:
			sml_close_response_write(message_body->d.close_response, buf);
			break;
		case SML_MESSAGE_GET_PROFILE_PACK_REQUEST:
			sml_get_profile_pack_request_write(message_body->d.get_profile_pack_request, buf);
			break;
		case SML_MESSAGE_GET_PROFILE_PACK_RESPONSE:
			sml_get_profile_pack_response_write(message_body->d.get_profile_pack_response, buf);
			break;
		case SML_MESSAGE_GET_PROFILE_LIST_REQUEST:
			sml_get_profile_list_request_write(message_body->d.get_profile_list_request, buf);
			break;
		case SML_MESSAGE_GET_PROFILE_LIST_RESPONSE:
			sml_get_profile_list_response_write(message_body->d.get_profile_list_response, buf);
			break;
		case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
			sml_get_proc_parameter_request_write(message_body->d.get_proc_parameter_request, buf);
			break;
		case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
			sml_get_proc_parameter_response_write(message_body->d.get_proc_parameter_response, buf);
			break;
		case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
			sml_set_proc_parameter_request_write(message_body->d.set_proc_parameter_request, buf);
			break;
		case SML_MESSAGE_GET_LIST_REQUEST:
			sml_get_list_request_write(message_body->d.get_list_request, buf);
			break;
		case SML_MESSAGE_GET_LIST_RESPONSE:
			sml_get_list_response_write(message_body->d.get_list_response, buf);
			break;
		case SML_MESSAGE_ATTENTION_RESPONSE:
			sml_attention_response_write(message_body->d.attention_response, buf);
			break;
		default:
			buf->error = SML_MESSAGE_TAG_MISMATCH;
			if (sml_debug_output(buf)) {
				trace("error: message type %04X not yet implemented\n", *(message_body->tag));
			}
			break;
	}
}

void sml_message_body_free(sml_message_body *message_body) {
	if (message_body) {
		if (message_body->tag) {
			switch (*(message_body->tag)) {
				case SML_MESSAGE_OPEN_REQUEST:
					sml_open_request_free(message_body->d.open_request);
					break;
				case SML_MESSAGE_OPEN_RESPONSE:
					sml_open_response_free(message_body->d.open_response);
					break;
				case SML_MESSAGE_CLOSE_REQUEST:
					sml_close_request_free(message_body->d.close_request);
					break;
				case SML_MESSAGE_CLOSE_RESPONSE:
					sml_close_response_free(message_body->d.close_response);
					break;
				case SML_MESSAGE_GET_PROFILE_PACK_REQUEST:
					sml_get_profile_pack_request_free(message_body->d.get_profile_pack_request);
					break;
				case SML_MESSAGE_GET_PROFILE_PACK_RESPONSE:
					sml_get_profile_pack_response_free(message_body->d.get_profile_pack_response);
					break;
				case SML_MESSAGE_GET_PROFILE_LIST_REQUEST:
					sml_get_profile_list_request_free(message_body->d.get_profile_list_request);
					break;
				case SML_MESSAGE_GET_PROFILE_LIST_RESPONSE:
					sml_get_profile_list_response_free(message_body->d.get_profile_list_response);
					break;
				case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
					sml_get_proc_parameter_request_free(message_body->d.get_proc_parameter_request);
					break;
				case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
					sml_get_proc_parameter_response_free(message_body->d.get_proc_parameter_response);
					break;
				case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
					sml_set_proc_parameter_request_free(message_body->d.set_proc_parameter_request);
					break;
				case SML_MESSAGE_GET_LIST_REQUEST:
					sml_get_list_request_free(message_body->d.get_list_request);
					break;
				case SML_MESSAGE_GET_LIST_RESPONSE:
					sml_get_list_response_free(message_body->d.get_list_response);
					break;
				case SML_MESSAGE_ATTENTION_RESPONSE:
					sml_attention_response_free(message_body->d.attention_response);
					break;
				default:
					trace("NYI: %s for message type %04X\n", __FUNCTION__, *(message_body->tag));
					break;
			}
			sml_number_free(message_body->tag);
		}
		free(message_body);
	}
}

