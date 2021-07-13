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
#include <sml/sml_boolean.h>
#include <sml/sml_log.h>

// sml_boolean;

sml_boolean *sml_boolean_init(u8 b) {
	sml_boolean *boolean = (sml_boolean*)malloc(sizeof(u8));
	if (!boolean) {
		return NULL;
	}

	*boolean = b;

	return boolean;
}

sml_boolean *sml_boolean_parse(sml_buffer *buf) {
	if (sml_buf_optional_is_skipped(buf)) {
		return 0;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_BOOLEAN) {
		buf->error = SML_TYPE_BOOLEAN_EXPECTED;
		return 0;
	}

	if (sml_buf_get_next_length(buf) != 1) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		return 0;
	}

	u8 v;
	if (sml_buf_get_current_byte(buf)) {
		sml_buf_update_bytes_read(buf, 1);
		v = SML_BOOLEAN_TRUE;
	}
	else {
		sml_buf_update_bytes_read(buf, 1);
		v = SML_BOOLEAN_FALSE;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tBOOLEAN\t%i\r\n", v);
	}

	sml_boolean *boolean = sml_boolean_init(v);
	if (!boolean) {
		trace("ERROR: malloc %ld bytes\n", sizeof(u8));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	return boolean;
}

void sml_boolean_write(sml_boolean *boolean, sml_buffer *buf) {
	if (boolean == 0) {
		sml_buf_optional_write(buf);
		return;
	}

	sml_buf_set_type_and_length(buf, SML_TYPE_BOOLEAN, 1);
	if (*boolean == SML_BOOLEAN_FALSE) {
		buf->buffer[buf->cursor] = SML_BOOLEAN_FALSE;
	}
	else {
		buf->buffer[buf->cursor] = SML_BOOLEAN_TRUE;
	}
	buf->cursor++;
}

void sml_boolean_free(sml_boolean *b) {
	if (b) {
		free(b);
	}
}

