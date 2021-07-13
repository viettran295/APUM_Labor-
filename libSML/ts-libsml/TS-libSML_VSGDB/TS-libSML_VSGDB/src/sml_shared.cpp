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
#include <string.h>
#include <sml/sml_shared.h>
#include <sml/sml_log.h>

#ifdef USE_HAL_DRIVER
void *hal_driver_realloc(void *origPtr, size_t newSize) {
	void *newPtr = malloc(newSize);
	if (!newPtr)
	{
		trace("ERROR: malloc %ld bytes\n", newSize);
		return NULL;
	}
	if (origPtr)
	{
		size_t origSize = (newSize/sizeof(void*) - 1) * sizeof(void*);
		memcpy(newPtr, origPtr, origSize);
		free(origPtr);
	}
	return newPtr;
}
#endif

int sml_buf_get_next_length(sml_buffer *buf) {
	int length = 0;
	unsigned char byte = sml_buf_get_current_byte(buf);
	int list = ((byte & SML_TYPE_FIELD) == SML_TYPE_LIST) ? 0 : -1;

	if (sml_debug_output(buf)) {
		trace("%i\t", buf->cursor);
	}

	for (;buf->cursor < (int)buf->buffer_len;) {
		byte = sml_buf_get_current_byte(buf);
		length <<= 4;
		length |= (byte & SML_LENGTH_FIELD);

		if ((byte & SML_ANOTHER_TL) != SML_ANOTHER_TL) {
			break;
		}
		sml_buf_update_bytes_read(buf, 1);
		if(list) {
			list += -1;
		}
	}
	sml_buf_update_bytes_read(buf, 1);

	if (sml_debug_output(buf)) {
		trace("%i\r\n", length + list);
	}

	return length + list;
}

void sml_buf_set_type_and_length(sml_buffer *buf, unsigned int type, unsigned int l) {
	// set the type
	buf->buffer[buf->cursor] = type;

	if (type != SML_TYPE_LIST) {
		l++;
	}

	if (l > SML_LENGTH_FIELD) {

		// how much shifts are necessary
		int mask_pos = (sizeof(unsigned int) * 2) - 1;

		// the 4 most significant bits of l (1111 0000 0000 ...)
		unsigned int mask = 0xF0 << (8 * (sizeof(unsigned int) - 1));

		// select the next 4 most significant bits with a bit set until there 
		// is something
		while (!(mask & l)) {
			mask >>= 4;
			mask_pos--;
		}

		l += mask_pos; // for every TL-field

		//TODO: check this correction (reason not understood) - Uwe Heuert
		if (type == SML_TYPE_LIST) {
			l--;
		}

		if ((0x0F << (4 * (mask_pos + 1))) & l) {
			// for the rare case that the addition of the number of TL-fields
			// result in another TL-field.
			mask_pos++;
			l++;
		}

		// copy 4 bits of the number to the buffer
		while (mask > SML_LENGTH_FIELD) {
			buf->buffer[buf->cursor] |= SML_ANOTHER_TL;
			buf->buffer[buf->cursor] |= ((mask & l) >> (4 * mask_pos));
			mask >>= 4;
			mask_pos--;
			buf->cursor++;
		}
	}

	buf->buffer[buf->cursor] |= (l & SML_LENGTH_FIELD);
	buf->cursor++;
}

int sml_buf_has_errors(sml_buffer *buf) {
	return buf->error != SML_OK;
}

int sml_buf_get_next_type(sml_buffer *buf) {
	return (buf->buffer[buf->cursor] & SML_TYPE_FIELD);
}

unsigned char sml_buf_get_current_byte(sml_buffer *buf) {
	return buf->buffer[buf->cursor];
}

unsigned char *sml_buf_get_current_buf(sml_buffer *buf) {
	return &(buf->buffer[buf->cursor]);
}

void sml_buf_update_bytes_read(sml_buffer *buf, int bytes) {
	buf->cursor += bytes;
}

sml_buffer *sml_buffer_init(size_t length, int flags) {
	sml_buffer *buf = (sml_buffer *)malloc(sizeof(sml_buffer));
	if (!buf) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_buffer));
		return NULL;
	}

	memset(buf, 0, sizeof(sml_buffer));
	buf->buffer = (unsigned char *)malloc(length);
	if (!buf->buffer) {
		trace("ERROR: malloc %ld bytes\n", length);
		buf->error = SML_MALLOC_ERROR;
		goto error;
	}

	buf->buffer_len = length;
	memset(buf->buffer, 0, buf->buffer_len);

	buf->flags = flags;

	return buf;

error:
	sml_buffer_free(buf);
	return 0;
}

void sml_buf_optional_write(sml_buffer *buf) {
	buf->buffer[buf->cursor] = SML_OPTIONAL_SKIPPED;
	buf->cursor++;
}

void sml_buffer_free(sml_buffer *buf) {
	if (buf) {
		if (buf->buffer)
			free(buf->buffer);
		if (buf->error_msg)
			free(buf->error_msg);
		free(buf);
	}
}

int sml_buf_optional_is_skipped(sml_buffer *buf) {
	if (sml_buf_get_current_byte(buf) == SML_OPTIONAL_SKIPPED) {
		sml_buf_update_bytes_read(buf, 1);

		return 1;
	}

	return 0;
}

void hexdump(unsigned char *buffer, size_t buffer_len) {
	int i;
	for (i = 0; i < (int)buffer_len; i++) {
		trace("%02X ", (unsigned char) buffer[i]);
		if ((i + 1) % 8 == 0) {
			trace("\n");
		}
	}
	trace("\n");
}

int sml_debug_output(sml_buffer *buf) {
	return buf->flags & SML_FLAGS_DEBUG_OUTPUT;
}

char *sml_get_errorstring(int error)
{
	static struct
	{
		int code;
		char *text;
	} ct[] = {

		{ SML_OK,										"SML_OK" },

		{ SML_TYPE_OCTET_STRING_EXPECTED,				"SML_TYPE_OCTET_STRING_EXPECTED" },
		{ SML_TYPE_OCTET_STRING_LENGTH_OUT_OF_RANGE,	"SML_TYPE_OCTET_STRING_LENGTH_OUT_OF_RANGE" },

		{ SML_TYPE_BOOLEAN_EXPECTED,					"SML_TYPE_BOOLEAN_EXPECTED" },

		{ SML_TYPE_NUMBER_MISMATCH,						"SML_TYPE_NUMBER_MISMATCH" },
		{ SML_TYPE_NUMBER_LENGTH_OUT_OF_RANGE,			"SML_TYPE_NUMBER_LENGTH_OUT_OF_RANGE" },
		{ SML_EXPECTED_LENGTH_NOT_FOUND,				"SML_EXPECTED_LENGTH_NOT_FOUND" },

		{ SML_TYPE_LIST_EXPECTED,						"SML_TYPE_LIST_EXPECTED" },

		{ SML_STATUS_TAG_MISMATCH,						"SML_STATUS_TAG_MISMATCH" },
		{ SML_TIME_TAG_MISMATCH,						"SML_TIME_TAG_MISMATCH" },
		{ SML_PROC_PAR_VALUE_TAG_MISMATCH,				"SML_PROC_PAR_VALUE_TAG_MISMATCH" },
		{ SML_SIMPLE_VALUE_TAG_MISMATCH,				"SML_SIMPLE_VALUE_TAG_MISMATCH" },
		{ SML_COSEM_VALUE_TAG_MISMATCH,					"SML_COSEM_VALUE_TAG_MISMATCH" },
		{ SML_LIST_TYPE_TAG_MISMATCH,					"SML_LIST_TYPE_TAG_MISMATCH" },
		{ SML_VALUE_TAG_MISMATCH,						"SML_VALUE_TAG_MISMATCH" },
		{ SML_MESSAGE_TAG_MISMATCH,						"SML_MESSAGE_TAG_MISMATCH" },

		{ SML_MESSAGE_CRC_ERROR,						"SML_MESSAGE_CRC_ERROR" },

		{ SML_MALLOC_ERROR,								"SML_MALLOC_ERROR" },

			}, *p;

	for (p=ct; p < &ct[sizeof(ct)/sizeof(ct[0])]; p++)
	{
		if (p->code == error)
		{
			return p->text;
		}
	}

	return "(UNKNOWN)";
}
