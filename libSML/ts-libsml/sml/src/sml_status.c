// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_status.h>
#include <sml/sml_log.h>

// sml_status;

sml_status *sml_status_init(void)
{
	sml_status *status = (sml_status *)malloc(sizeof(sml_status));
	if (!status)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_status));
		return NULL;
	}
	memset(status, 0, sizeof(sml_status));
	
	return status;
}

sml_status *sml_status_parse(sml_buffer *buf)
{
    int max = 1;
    u8 type;
    unsigned char byte;
    sml_status *status = NULL;

	if (!buf)
	{
		return NULL;
	}

    if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	type = sml_buf_get_next_type(buf);
	byte = sml_buf_get_current_byte(buf);

	if (sml_debug_output(buf))
    {
		trace("\t\tSTATUS\t%i\r\n", type);
	}

	status = sml_status_init();
	if (!status)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	status->type = type;
	switch (type)
    {
		case SML_TYPE_UNSIGNED:
			// get maximal size, if not all bytes are used (example: only 6 bytes for a u64)
			while (max < ((byte & SML_LENGTH_FIELD) - 1))
            {
				max <<= 1;
			}

			status->data.status8 = (u8*)sml_number_parse(buf, type, max);
			status->type |= max;
			break;
		default:
			buf->error = SML_STATUS_TAG_MISMATCH;
			break;
	}
	if (sml_buf_has_errors(buf))
    {
		sml_status_free(status);
		return NULL;
	}

	return status;
}

int sml_status_write(sml_status *status, sml_buffer *buf)
{
	int rv = SML_OK;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (status == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_number_write(status->data.status8, (status->type & SML_TYPE_FIELD), (status->type & SML_LENGTH_FIELD), buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_status_free(sml_status *status)
{
	if (status)
    {
		sml_number_free(status->data.status8);
		free(status);
	}
}
