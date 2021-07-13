// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <sml/sml_value.h>
#include <stdio.h>
#include <sml/sml_log.h>

// sml_value;

sml_value *sml_value_init(void)
{
	sml_value *value = (sml_value *)malloc(sizeof(sml_value));
	if (!value)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_value));
		return NULL;
	}
	memset(value, 0, sizeof(sml_value));
	
	return value;
}

sml_value *sml_value_parse(sml_buffer *buf)
{
    int max = 1;
    u8 type;
    unsigned char byte;
    sml_value *value = NULL;
    
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

	value = sml_value_init();
	if (!value)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	value->type = type;

	if (sml_debug_output(buf))
    {
		trace("\t\tVALUE\t%i\r\n", type);
	}

	switch (type)
    {
		case SML_TYPE_OCTET_STRING:
			value->data.bytes = sml_octet_string_parse(buf);
			break;
		case SML_TYPE_BOOLEAN:
			value->data.boolean = sml_boolean_parse(buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			// get maximal size, if not all bytes are used (example: only 6 bytes for a u64)
			while (max < ((byte & SML_LENGTH_FIELD) - 1))
            {
				max <<= 1;
			}

			value->data.uint8 = (u8*)sml_number_parse(buf, type, max);
			value->type |= max;
			break;
		case SML_TYPE_LIST:
			value->data.smlList = sml_list_type_parse(buf);
			break;
		default:
			buf->error = SML_VALUE_TAG_MISMATCH;
			break;
	}

	if (sml_buf_has_errors(buf))
    {
		sml_value_free(value);
		return NULL;
	}

	return value;
}

int sml_value_write(sml_value *value, sml_buffer *buf)
{
	int rv = SML_OK;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (value == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	switch (value->type & SML_TYPE_FIELD)
    {
		case SML_TYPE_OCTET_STRING:
			rv = sml_octet_string_write(value->data.bytes, buf);
			break;
		case SML_TYPE_BOOLEAN:
			rv = sml_boolean_write(value->data.boolean, buf);
			break;
		case SML_TYPE_UNSIGNED:
		case SML_TYPE_INTEGER:
			rv = sml_number_write(value->data.uint8, (value->type & SML_TYPE_FIELD),
				(value->type & SML_LENGTH_FIELD), buf);
			break;
		case SML_TYPE_LIST:
			rv = sml_list_type_write(value->data.smlList, buf);
			break;
	}

	return rv;
}

void sml_value_free(sml_value *value)
{
	if (value)
    {
		switch (value->type)
        {
			case SML_TYPE_OCTET_STRING:
				sml_octet_string_free(value->data.bytes);
				break;
			case SML_TYPE_BOOLEAN:
				sml_boolean_free(value->data.boolean);
				break;
			case SML_TYPE_LIST:
				sml_list_type_free(value->data.smlList);
				break;
			default:
				sml_number_free(value->data.int8);
				break;
		}
		free(value);
	}
}

double sml_value_to_double(sml_value *value)
{
	switch (value->type)
    {
		case 0x51: return *value->data.int8;   break;
		case 0x52: return *value->data.int16;  break;
		case 0x54: return *value->data.int32;  break;
		case 0x58: return (double)*value->data.int64;  break;
		case 0x61: return *value->data.uint8;  break;
		case 0x62: return *value->data.uint16; break;
		case 0x64: return *value->data.uint32; break;
		case 0x68: return (double)*value->data.uint64; break;

		default:
			trace("error: unknown type in %s\n", __FUNCTION__);
			return 0;
	}
}
