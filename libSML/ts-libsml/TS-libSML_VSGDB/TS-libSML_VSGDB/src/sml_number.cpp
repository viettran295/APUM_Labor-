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
#include <sml/sml_number.h>
#include <sml/sml_log.h>

// sml_number;

void *sml_number_init(u64 number, unsigned char type, int size) {
	
	unsigned char* bytes = (unsigned char*)&number;

	// Swap bytes of big-endian number so that
	// memcpy copies the right part
	if (sml_number_endian() == SML_BIG_ENDIAN) {
		  bytes += sizeof(u64) - size;
	}

	unsigned char *np = (unsigned char *)malloc(size);
	if (!np) {
		trace("ERROR: malloc %d bytes\n", size);
		return NULL;
	}

	memset(np, 0, size);
	memcpy(np, bytes, size);

	return np;
}

void *sml_number_parse(sml_buffer *buf, unsigned char type, int max_size) {
	if (sml_buf_optional_is_skipped(buf)) {
		return 0;
	}

	int l, i;
	unsigned char b;
	short negative_int = 0;

	if (sml_buf_get_next_type(buf) != type) {
		buf->error = SML_TYPE_NUMBER_MISMATCH;
		return 0;
	}

	l = sml_buf_get_next_length(buf);
	if (l < 0 || l > max_size) {
		buf->error = SML_TYPE_NUMBER_LENGTH_OUT_OF_RANGE;
		return 0;
	}

	unsigned char *np = (unsigned char *)malloc(max_size);
	if (!np) {
		trace("ERROR: malloc %d bytes\n", max_size);
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	memset(np, 0, max_size);


	b = sml_buf_get_current_byte(buf);
	if (type == SML_TYPE_INTEGER && (b & 128)) {
		negative_int = 1;
	}

	int missing_bytes = max_size - l;
	memcpy(&(np[missing_bytes]), sml_buf_get_current_buf(buf), l);

	if (negative_int) {
		for (i = 0; i < missing_bytes; i++) {
			np[i] = 0xFF;
		}
	}

	if (!(sml_number_endian() == SML_BIG_ENDIAN)) {
		sml_number_byte_swap(np, max_size);
	}
	sml_buf_update_bytes_read(buf, l);

	if (sml_debug_output(buf)) {
		trace("\t\tNUMBER\t%i\t%i\r\n", l, *np);
	}

	return np;
}

void sml_number_write(void *np, unsigned char type, int size, sml_buffer *buf) {
	if (np == 0) {
		sml_buf_optional_write(buf);
		return;
	}

	if (	(size > 1) &&
			(((type == SML_TYPE_UNSIGNED) && (buf->flags & SML_FLAGS_WRITE_NUMBER_UNSIGNED_DENSE)) ||
			((type == SML_TYPE_INTEGER) && ((buf->flags & SML_FLAGS_WRITE_NUMBER_SIGNED_DENSE) || (buf->flags & SML_FLAGS_WRITE_NUMBER_SIGNED_COMPACT))))) {
		int i, new_size = size;
		unsigned char *bytes = (unsigned char *)np;
		for (i = 0; i < size - 1; i++) {
			int pos = i;
			if (!(sml_number_endian() == SML_BIG_ENDIAN)) {
				pos = size - i - 1;
			}
			if (bytes[pos] == 0) {
				new_size--;
			}
			else {
				break;
			}
		}
		if (new_size != size) {
			if ((type == SML_TYPE_INTEGER) && (buf->flags & SML_FLAGS_WRITE_NUMBER_SIGNED_COMPACT)) {
				new_size++;
			}
			if (sml_number_endian() == SML_BIG_ENDIAN) {
				np = bytes + (size - new_size);
			}
			size = new_size;
		}
	}

	sml_buf_set_type_and_length(buf, type, size);
	memcpy(sml_buf_get_current_buf(buf), np, size);

	if (!(sml_number_endian() == SML_BIG_ENDIAN)) {
		sml_number_byte_swap(sml_buf_get_current_buf(buf), size);
	}

	sml_buf_update_bytes_read(buf, size);
}

void sml_number_byte_swap(unsigned char *bytes, int bytes_len) {
	int i;
	//unsigned char ob[bytes_len];
	//memcpy(&ob, bytes, bytes_len);

	unsigned char *ob = (unsigned char *)malloc(bytes_len);
	if (!ob) {
		trace("ERROR: malloc %d bytes\n", bytes_len);
		return;
	}

	memcpy(ob, bytes, bytes_len);
	
	for (i = 0; i < bytes_len; i++) {
		bytes[i] = ob[bytes_len - (i + 1)];
	}

	free(ob);
}

int sml_number_endian() {
	int i = 1;
	char *p = (char *)&i;

	if (p[0] == 1)
		return SML_LITTLE_ENDIAN;
	else
		return SML_BIG_ENDIAN;
}

void sml_number_free(void *np) {
	if (np) {
		free(np);
	}
}

