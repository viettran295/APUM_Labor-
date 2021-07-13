// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_time.h>
#include <sml/sml_shared.h>
#include <sml/sml_number.h>
#include <sml/sml_log.h>

// sml_timestamp_local;

sml_timestamp_local *sml_timestamp_local_init(void)
{
	sml_timestamp_local *timestamp_local = (sml_timestamp_local *)malloc(sizeof(sml_timestamp_local));
	if (!timestamp_local)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_timestamp_local));
		return NULL;
	}
	memset(timestamp_local, 0, sizeof(sml_timestamp_local));
	
	return timestamp_local;
}

sml_timestamp_local *sml_timestamp_local_parse(sml_buffer *buf)
{
    sml_timestamp_local *timestamp_local = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	timestamp_local = sml_timestamp_local_init();
	if (!timestamp_local)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 3)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tTIMESTAMP LOCAL\r\n");
	}

	timestamp_local->timestamp = sml_u32_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	timestamp_local->local_offset = sml_i16_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	timestamp_local->season_time_offset = sml_i16_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return timestamp_local;

error:
	sml_timestamp_local_free(timestamp_local);
	return NULL;
}

int sml_timestamp_local_write(sml_timestamp_local *timestamp_local, sml_buffer *buf)
{
	int rv;

	if (!timestamp_local || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);
	if (rv != SML_OK) return rv;

	rv = sml_u32_write(timestamp_local->timestamp, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i16_write(timestamp_local->local_offset, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i16_write(timestamp_local->season_time_offset, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_timestamp_local_free(sml_timestamp_local *timestamp_local)
{
    if (timestamp_local)
    {
		sml_number_free(timestamp_local->timestamp);
		sml_number_free(timestamp_local->local_offset);
		sml_number_free(timestamp_local->season_time_offset);
		free(timestamp_local);
    }
}

// sml_time;

sml_time *sml_time_init(void)
{
	sml_time *tme = (sml_time *)malloc(sizeof(sml_time));
	if (!tme)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_time));
		return NULL;
	}
	memset(tme, 0, sizeof(sml_time));
	
	return tme;
}

sml_time *sml_time_parse(sml_buffer *buf)
{
    sml_time *tme = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	tme = sml_time_init();
	if (!tme)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		if (buf->flags & SML_FLAGS_ISKRA_BZ_IGNORE_BUGS)
		{
			tme->tag = sml_u8_init(SML_TIME_SEC_INDEX);
			tme->data.timestamp = sml_u32_parse(buf);
			return tme;
		}
		else
		{
			buf->error = SML_TYPE_LIST_EXPECTED;
			goto error;
		}
	}

	if (sml_buf_get_next_length(buf) != 2)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tTIME\r\n");
	}

	tme->tag = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	if (!tme->tag) goto error;

	switch (*(tme->tag))
    {
		case SML_TIME_SEC_INDEX:
		case SML_TIME_TIMESTAMP:
			tme->data.timestamp = sml_u32_parse(buf);
			break;
		case SML_TIME_TIMESTAMP_LOCAL:
			tme->data.timestamp_local = sml_timestamp_local_parse(buf);
			break;
		default:
			buf->error = SML_TIME_TAG_MISMATCH;
			goto error;
	}

	return tme;

error:
	sml_time_free(tme);
	return NULL;
}

int sml_time_write(sml_time *t, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (t == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(t->tag, buf);
	if (rv != SML_OK) return rv;

	switch (*(t->tag))
    {
		case SML_TIME_SEC_INDEX:
		case SML_TIME_TIMESTAMP:
			rv = sml_u32_write(t->data.timestamp, buf);
			break;
		case SML_TIME_TIMESTAMP_LOCAL:
			rv = sml_timestamp_local_write(t->data.timestamp_local, buf);
			break;
	}

	return rv;
}

void sml_time_free(sml_time *tme)
{
    if (tme)
    {
		if (tme->tag)
        {
			switch (*(tme->tag))
            {
				case SML_TIME_SEC_INDEX:
				case SML_TIME_TIMESTAMP:
					sml_number_free(tme->data.timestamp);
					break;
				case SML_TIME_TIMESTAMP_LOCAL:
					sml_timestamp_local_free(tme->data.timestamp_local);
					break;
			}
			sml_number_free(tme->tag);
		}
		free(tme);
    }
}
