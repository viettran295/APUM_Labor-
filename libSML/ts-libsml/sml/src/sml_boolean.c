// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_boolean.h>
#include <sml/sml_log.h>

// sml_boolean;

sml_boolean *sml_boolean_init(u8 b)
{
	sml_boolean *boolean = (sml_boolean*)malloc(sizeof(u8));
	if (!boolean)
    {
		trace("ERROR: malloc %ld bytes\n", sizeof(u8));
		return NULL;
	}

	*boolean = b;

	return boolean;
}

sml_boolean *sml_boolean_parse(sml_buffer *buf)
{
    u8 v;
    sml_boolean *boolean = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_BOOLEAN)
    {
		buf->error = SML_TYPE_BOOLEAN_EXPECTED;
		return NULL;
	}

	if (sml_buf_get_next_length(buf) != 1)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		return NULL;
	}

	if (sml_buf_get_current_byte(buf))
    {
		sml_buf_update_bytes_read(buf, 1);
		v = SML_BOOLEAN_TRUE;
	}
	else
    {
		sml_buf_update_bytes_read(buf, 1);
		v = SML_BOOLEAN_FALSE;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tBOOLEAN\t%i\r\n", v);
	}

	boolean = sml_boolean_init(v);
	if (!boolean)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	return boolean;
}

int sml_boolean_write(sml_boolean *boolean, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (boolean == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_BOOLEAN, 1);
	if (rv != SML_OK) return rv;

	if (*boolean == SML_BOOLEAN_FALSE)
    {
		buf->buffer[buf->cursor] = SML_BOOLEAN_FALSE;
	}
	else
    {
		buf->buffer[buf->cursor] = SML_BOOLEAN_TRUE;
	}
	buf->cursor++;

	return rv;
}

void sml_boolean_free(sml_boolean *b)
{
	if (b)
    {
		free(b);
	}
}
