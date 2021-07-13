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
#include <sml/sml_time.h>
#include <sml/sml_shared.h>
#include <sml/sml_number.h>
#include <sml/sml_log.h>

// sml_timestamp_local;

sml_timestamp_local *sml_timestamp_local_init() {
	sml_timestamp_local *timestamp_local = (sml_timestamp_local *)malloc(sizeof(sml_timestamp_local));
	if (timestamp_local) {
		memset(timestamp_local, 0, sizeof(sml_timestamp_local));
	}
	return timestamp_local;
}

sml_timestamp_local *sml_timestamp_local_parse(sml_buffer *buf) {
	if (sml_buf_optional_is_skipped(buf)) {
		return 0;
	}

	sml_timestamp_local *timestamp_local = sml_timestamp_local_init();
	if (!timestamp_local) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_timestamp_local));
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
	return 0;
}

void sml_timestamp_local_write(sml_timestamp_local *timestamp_local, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);

	sml_u32_write(timestamp_local->timestamp, buf);
	sml_i16_write(timestamp_local->local_offset, buf);
	sml_i16_write(timestamp_local->season_time_offset, buf);
}

void sml_timestamp_local_free(sml_timestamp_local *timestamp_local) {
    if (timestamp_local) {
		sml_number_free(timestamp_local->timestamp);
		sml_number_free(timestamp_local->local_offset);
		sml_number_free(timestamp_local->season_time_offset);
		free(timestamp_local);
    }
}

// sml_time;

sml_time *sml_time_init() {
	sml_time *tme = (sml_time *)malloc(sizeof(sml_time));
	if (tme) {
		memset(tme, 0, sizeof(sml_time));
	}

	

	return tme;
}

sml_time *sml_time_parse(sml_buffer *buf) {
	if (sml_buf_optional_is_skipped(buf)) {
		return 0;
	}

	sml_time *tme = sml_time_init();
	if (!tme) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_time));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
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

	if (sml_buf_get_next_length(buf) != 2) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tTIME\r\n");
	}

	tme->tag = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	if (!tme->tag) goto error;

	switch (*(tme->tag)) {
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
	return 0;
}

void sml_time_write(sml_time *t, sml_buffer *buf) {
	if (t == 0) {
		sml_buf_optional_write(buf);
		return;
	}

	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	sml_u8_write(t->tag, buf);
	switch (*(t->tag)) {
		case SML_TIME_SEC_INDEX:
		case SML_TIME_TIMESTAMP:
			sml_u32_write(t->data.timestamp, buf);
			break;
		case SML_TIME_TIMESTAMP_LOCAL:
			sml_timestamp_local_write(t->data.timestamp_local, buf);
			break;
	}
}

void sml_time_free(sml_time *tme) {
    if (tme) {
		if (tme->tag) {
			switch (*(tme->tag)) {
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

