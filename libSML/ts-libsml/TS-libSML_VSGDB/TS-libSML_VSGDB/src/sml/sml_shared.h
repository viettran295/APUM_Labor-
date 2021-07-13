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

#ifndef _SML_SHARED_H_
#define	_SML_SHARED_H_

#include <stdlib.h>

#ifdef USE_HAL_DRIVER
	#include <FreeRTOS.h>
	#undef malloc
	#define malloc pvPortMalloc
	#undef free
	#define free vPortFree
	#undef realloc
	//#define realloc hal_driver_realloc
	#define realloc pvPortRealloc
#endif

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define SML_MESSAGE_END				0x0

#define SML_TYPE_FIELD				0x70
#define SML_LENGTH_FIELD			0xF
#define SML_ANOTHER_TL				0x80

#define SML_TYPE_OCTET_STRING		0x0
#define SML_TYPE_BOOLEAN			0x40
#define SML_TYPE_INTEGER			0x50
#define SML_TYPE_UNSIGNED			0x60
#define SML_TYPE_LIST				0x70

#define SML_OPTIONAL_SKIPPED		0x1

#define SML_TYPE_NUMBER_8			sizeof(u8)
#define SML_TYPE_NUMBER_16			sizeof(u16)
#define SML_TYPE_NUMBER_32			sizeof(u32)
#define SML_TYPE_NUMBER_64			sizeof(u64)

#define SML_FLAGS_NONE							0x0000
#define SML_FLAGS_WRITE_CHOICE_COMPACT			0x0001
#define SML_FLAGS_WRITE_NUMBER_UNSIGNED_DENSE	0x0002
#define SML_FLAGS_WRITE_NUMBER_SIGNED_COMPACT	0x0004
#define SML_FLAGS_WRITE_NUMBER_SIGNED_DENSE		0x0008
#define SML_FLAGS_DEBUG_OUTPUT					0x0100

#define SML_FLAGS_ISKRA_BZ_IGNORE_BUGS			0x1000

#define SML_OK										0

#define SML_TYPE_OCTET_STRING_EXPECTED				10
#define SML_TYPE_OCTET_STRING_LENGTH_OUT_OF_RANGE	11

#define SML_TYPE_BOOLEAN_EXPECTED					20

#define SML_TYPE_NUMBER_MISMATCH					30
#define SML_TYPE_NUMBER_LENGTH_OUT_OF_RANGE			31
#define SML_EXPECTED_LENGTH_NOT_FOUND				32

#define SML_TYPE_LIST_EXPECTED						50

#define SML_STATUS_TAG_MISMATCH						100
#define SML_TIME_TAG_MISMATCH						101
#define SML_PROC_PAR_VALUE_TAG_MISMATCH				102
#define SML_SIMPLE_VALUE_TAG_MISMATCH				103
#define SML_COSEM_VALUE_TAG_MISMATCH				104
#define SML_LIST_TYPE_TAG_MISMATCH					105
#define SML_VALUE_TAG_MISMATCH						106
#define SML_MESSAGE_TAG_MISMATCH					107

#define SML_MESSAGE_CRC_ERROR						200

#define SML_MALLOC_ERROR							999

// This sml_buffer is used in two different use-cases.
//
// Parsing: the raw data is in the buffer field,
//          the buffer_len is the number of raw bytes received,
//          the cursor points to the current position during parsing
//
// Writing: At the beginning the buffer field is malloced and zeroed with
//          a default length, this default length is stored in buffer_len
//          (i.e. the maximum bytes one can write to this buffer)
//          cursor points to the position, where on can write during the
//          writing process. If a file is completely written to the buffer,
//          cursor is the number of bytes written to the buffer.
typedef struct {
	unsigned char *buffer;
	size_t buffer_len;
	int cursor;
	int flags;
	int error;
	char *error_msg;
} sml_buffer;

sml_buffer *sml_buffer_init(size_t length, int flags);

void sml_buffer_free(sml_buffer *buf);

// Returns the length of the following data structure. Sets the cursor position to
// the value field.
int sml_buf_get_next_length(sml_buffer *buf);

void sml_buf_set_type_and_length(sml_buffer *buf, unsigned int type, unsigned int l);

// Checks if a error is occured.
int sml_buf_has_errors(sml_buffer *buf);

// Returns the type field of the current byte.
int sml_buf_get_next_type(sml_buffer *buf);

// Returns the current byte.
unsigned char sml_buf_get_current_byte(sml_buffer *buf);

// Returns a pointer to the current buffer position.
unsigned char *sml_buf_get_current_buf(sml_buffer *buf);

void sml_buf_optional_write(sml_buffer *buf);

// Sets the number of bytes read (moves the cursor forward)
void sml_buf_update_bytes_read(sml_buffer *buf, int bytes);

// Checks if the next field is a skipped optional field, updates the buffer accordingly
int sml_buf_optional_is_skipped(sml_buffer *buf);

// Prints arbitrarily byte string to stdout with printf
void hexdump(unsigned char *buffer, size_t buffer_len);

// Returns if debug output on stdout is on.
int sml_debug_output(sml_buffer *buf);

// Returns error string.
char *sml_get_errorstring(int error);

#ifdef USE_HAL_DRIVER
void *hal_driver_realloc(void *origPtr, size_t newSize);
#endif // #ifdef USE_HAL_DRIVER

#ifdef __cplusplus
}
#endif


#endif /* _SML_SHARED_H_ */

