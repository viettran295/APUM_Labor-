// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_v105.h>
#include <sml/sml_log.h>

// sml_simple_value;

sml_simple_value *sml_simple_value_init(void)
{
	sml_simple_value *simple_value = (sml_simple_value *)malloc(sizeof(sml_simple_value));
	if (!simple_value)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_simple_value));
		return NULL;
	}
	memset(simple_value, 0, sizeof(sml_simple_value));
	
	return simple_value;
}

sml_simple_value *sml_simple_value_parse(sml_buffer *buf)
{
   	int max = 1;
	u8 type;
	unsigned char byte;
    sml_simple_value *simple_value = NULL;

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

	simple_value = sml_simple_value_init();
	if (!simple_value)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	simple_value->type = type;

	if (sml_debug_output(buf))
    {
		trace("\t\tSIMPLE VALUE\t%i\r\n", type);
	}

	switch (type)
    {
		case SML_TYPE_OCTET_STRING:
			simple_value->data.bytes = sml_octet_string_parse(buf);
			break;
		case SML_TYPE_BOOLEAN:
			simple_value->data.boolean = sml_boolean_parse(buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			// get maximal size, if not all bytes are used (example: only 6 bytes for a u64)
			while (max < ((byte & SML_LENGTH_FIELD) - 1))
            {
				max <<= 1;
			}

			simple_value->data.uint8 = (u8*)sml_number_parse(buf, type, max);
			simple_value->type |= max;
			break;
		default:
			buf->error = SML_SIMPLE_VALUE_TAG_MISMATCH;
			break;
	}

	if (sml_buf_has_errors(buf))
    {
		sml_simple_value_free(simple_value);
		return NULL;
	}

	return simple_value;
}

int sml_simple_value_write(sml_simple_value *simple_value, sml_buffer *buf)
{
	int rv = SML_OK;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (simple_value == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	switch (simple_value->type & SML_TYPE_FIELD)
    {
		case SML_TYPE_OCTET_STRING:
			rv = sml_octet_string_write(simple_value->data.bytes, buf);
			break;
		case SML_TYPE_BOOLEAN:
			rv = sml_boolean_write(simple_value->data.boolean, buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			rv = sml_number_write(simple_value->data.uint8, (simple_value->type & SML_TYPE_FIELD),
				(simple_value->type & SML_LENGTH_FIELD), buf);
			break;
	}

	return rv;
}

void sml_simple_value_free(sml_simple_value *simple_value)
{
	if (simple_value)
    {
		switch (simple_value->type)
        {
			case SML_TYPE_OCTET_STRING:
				sml_octet_string_free(simple_value->data.bytes);
				break;
			case SML_TYPE_BOOLEAN:
				sml_boolean_free(simple_value->data.boolean);
				break;
			default:
				sml_number_free(simple_value->data.int8);
				break;
		}
		free(simple_value);
	}
}

// sml_timestamped_value;

sml_timestamped_value *sml_timestamped_value_init(void)
{
	sml_timestamped_value *timestamped_value = (sml_timestamped_value *)malloc(sizeof(sml_timestamped_value));
	if (!timestamped_value)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_timestamped_value));
		return NULL;
	}
	memset(timestamped_value, 0, sizeof(sml_timestamped_value));
	
	return timestamped_value;
}

sml_timestamped_value *sml_timestamped_value_parse(sml_buffer *buf)
{
    sml_timestamped_value *timestamped_value = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	timestamped_value = sml_timestamped_value_init();
	if (!timestamped_value)
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
		trace("\t\tTIMESTAMPED VALUE\r\n");
	}

	timestamped_value->time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	timestamped_value->status = sml_status_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	timestamped_value->simple_value = sml_simple_value_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return timestamped_value;

error:
	sml_timestamped_value_free(timestamped_value);
	return NULL;
}

int sml_timestamped_value_write(sml_timestamped_value *timestamped_value, sml_buffer *buf)
{
	int rv;

	if (!timestamped_value || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(timestamped_value->time, buf);
	if (rv != SML_OK) return rv;

	rv = sml_status_write(timestamped_value->status, buf);
	if (rv != SML_OK) return rv;

	rv = sml_simple_value_write(timestamped_value->simple_value, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_timestamped_value_free(sml_timestamped_value *timestamped_value)
{
    if (timestamped_value)
    {
		sml_time_free(timestamped_value->time);
		sml_status_free(timestamped_value->status);
		sml_simple_value_free(timestamped_value->simple_value);
        free(timestamped_value);
    }
}

// sml_cosem_scaler_unit;

sml_cosem_scaler_unit *sml_cosem_scaler_unit_init(void)
{
	sml_cosem_scaler_unit *cosem_scaler_unit = (sml_cosem_scaler_unit *)malloc(sizeof(sml_cosem_scaler_unit));
	if (!cosem_scaler_unit)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_cosem_scaler_unit));
		return NULL;
	}
	memset(cosem_scaler_unit, 0, sizeof(sml_cosem_scaler_unit));
	
	return cosem_scaler_unit;
}

sml_cosem_scaler_unit *sml_cosem_scaler_unit_parse(sml_buffer *buf)
{
    sml_cosem_scaler_unit *cosem_scaler_unit = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	cosem_scaler_unit = sml_cosem_scaler_unit_init();
	if (!cosem_scaler_unit)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 2)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tCOSEM SCALER UNIT\r\n");
	}

	cosem_scaler_unit->scaler = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	cosem_scaler_unit->unit = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return cosem_scaler_unit;

error:
	sml_cosem_scaler_unit_free(cosem_scaler_unit);
	return NULL;
}

int sml_cosem_scaler_unit_write(sml_cosem_scaler_unit *cosem_scaler_unit, sml_buffer *buf)
{
	int rv;

	if (!cosem_scaler_unit || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(cosem_scaler_unit->scaler, buf);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(cosem_scaler_unit->unit, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_cosem_scaler_unit_free(sml_cosem_scaler_unit *cosem_scaler_unit)
{
    if (cosem_scaler_unit)
    {
		sml_number_free(cosem_scaler_unit->scaler);
		sml_number_free(cosem_scaler_unit->unit);
        free(cosem_scaler_unit);
    }
}

// sml_cosem_value;

sml_cosem_value *sml_cosem_value_init(void)
{
	sml_cosem_value *cosem_value = (sml_cosem_value *)malloc(sizeof(sml_cosem_value));
	if (!cosem_value)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_cosem_value));
		return NULL;
	}
	memset(cosem_value, 0, sizeof(sml_cosem_value));
	
	return cosem_value;
}

sml_cosem_value *sml_cosem_value_parse(sml_buffer *buf)
{
    sml_cosem_value *cosem_value = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	cosem_value = sml_cosem_value_init();
	if (!cosem_value)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 2)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	cosem_value->tag = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	if (!cosem_value->tag) goto error;

	if (sml_debug_output(buf))
    {
		trace("\t\tCOSEM VALUE\t%i\r\n", *(cosem_value->tag));
	}

	switch (*(cosem_value->tag))
    {
		case SML_COSEM_VALUE_SCALER_UNIT:
			cosem_value->data.scaler_unit = sml_cosem_scaler_unit_parse(buf);
			break;

		default:
			buf->error = SML_COSEM_VALUE_TAG_MISMATCH;
			goto error;
	}

	return cosem_value;

error:
	sml_cosem_value_free(cosem_value);
	return NULL;
}

int sml_cosem_value_write(sml_cosem_value *cosem_value, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (cosem_value == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(cosem_value->tag, buf);
	if (rv != SML_OK) return rv;

	switch (*(cosem_value->tag))
    {
		case SML_COSEM_VALUE_SCALER_UNIT:
			rv = sml_cosem_scaler_unit_write(cosem_value->data.scaler_unit, buf);
			break;
	}

	return rv;
}

void sml_cosem_value_free(sml_cosem_value *cosem_value)
{
    if (cosem_value)
    {
		if (cosem_value->tag)
        {
			switch (*(cosem_value->tag))
            {
				case SML_COSEM_VALUE_SCALER_UNIT:
					sml_cosem_scaler_unit_free(cosem_value->data.scaler_unit);
					break;
			}
			sml_number_free(cosem_value->tag);
		}
        free(cosem_value);
    }
}

// sml_list_type;

sml_list_type *sml_list_type_init(void)
{
	sml_list_type *list_type = (sml_list_type *)malloc(sizeof(sml_list_type));
	if (!list_type)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_list_type));
		return NULL;
	}
	memset(list_type, 0, sizeof(sml_list_type));
	
	return list_type;
}

sml_list_type *sml_list_type_parse(sml_buffer *buf)
{
    sml_list_type *list_type = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	list_type = sml_list_type_init();
	if (!list_type)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 2)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	list_type->tag = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	if (!list_type->tag) goto error;

	if (sml_debug_output(buf))
    {
		trace("\t\tLIST TYPE\t%i\r\n", *(list_type->tag));
	}

	switch (*(list_type->tag))
    {
		case SML_LIST_TYPE_TIME:
			list_type->data.time = sml_time_parse(buf);
			break;
		case SML_LIST_TYPE_TIMESTAMPED_VALUE:
			list_type->data.timestamped_value = sml_timestamped_value_parse(buf);
			break;
		case SML_LIST_TYPE_COSEM_VALUE:
			list_type->data.cosem_value = sml_cosem_value_parse(buf);
			break;

		default:
			buf->error = SML_LIST_TYPE_TAG_MISMATCH;
			goto error;
	}

	return list_type;

error:
	sml_list_type_free(list_type);
	return NULL;
}

int sml_list_type_write(sml_list_type *list_type, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (list_type == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(list_type->tag, buf);
	if (rv != SML_OK) return rv;

	switch (*(list_type->tag))
    {
		case SML_LIST_TYPE_TIME:
			rv = sml_time_write(list_type->data.time, buf);
			break;
		case SML_LIST_TYPE_TIMESTAMPED_VALUE:
			rv = sml_timestamped_value_write(list_type->data.timestamped_value, buf);
			break;
		case SML_LIST_TYPE_COSEM_VALUE:
			rv = sml_cosem_value_write(list_type->data.cosem_value, buf);
			break;
	}

	return rv;
}

void sml_list_type_free(sml_list_type *list_type)
{
    if (list_type)
    {
		if (list_type->tag)
        {
			switch (*(list_type->tag))
            {
				case SML_LIST_TYPE_TIME:
					sml_time_free(list_type->data.time);
					break;
				case SML_LIST_TYPE_TIMESTAMPED_VALUE:
					sml_timestamped_value_free(list_type->data.timestamped_value);
					break;
				case SML_LIST_TYPE_COSEM_VALUE:
					sml_cosem_value_free(list_type->data.cosem_value);
					break;
			}
			sml_number_free(list_type->tag);
		}
        free(list_type);
    }
}
