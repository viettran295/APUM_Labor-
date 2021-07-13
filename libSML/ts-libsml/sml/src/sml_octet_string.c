// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#ifndef USE_HAL_DRIVER
	#include <sys/stat.h>
	#include <fcntl.h>
	#ifndef WIN32
		#include <unistd.h>
	#endif
	#ifndef _NO_UUID_LIB
		#include <uuid/uuid.h>
	#else
		//#include <stdlib.h> // for rand()
	#endif
#endif

#include <sml/sml_octet_string.h>
#include <sml/sml_log.h>

uint8_t ctoi(uint8_t c);
uint8_t c2toi(uint8_t c1, uint8_t c2);
uint8_t c2ptoi(char* c);

// octet_string;

octet_string *sml_octet_string_init(unsigned char *str, int length)
{
	octet_string *s = (octet_string *)malloc(sizeof(octet_string));
	if (!s)
    {
		trace("ERROR: malloc %ld bytes\n", sizeof(octet_string));
		return NULL;
	}

	memset(s, 0, sizeof(octet_string));
	if (length > 0)
    {
		s->str = (unsigned char *)malloc(length + 1);
		if (!s->str)
        {
			trace("ERROR: malloc %d bytes\n", length + 1);
			return NULL;
		}
        memset(s->str, 0, length + 1);
		memcpy(s->str, str, length);
		s->len = length;
	}

	return s;
}

octet_string *sml_octet_string_parse(sml_buffer *buf)
{
	int l;
	octet_string *str = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
	{
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_OCTET_STRING)
	{
		buf->error = SML_TYPE_OCTET_STRING_EXPECTED;
		return NULL;
	}

	l = sml_buf_get_next_length(buf);
	if (l < 0)
	{
		buf->error = SML_TYPE_OCTET_STRING_LENGTH_OUT_OF_RANGE;
		return NULL;
	}

	str = sml_octet_string_init(sml_buf_get_current_buf(buf), l);
	sml_buf_update_bytes_read(buf, l);

	if (sml_debug_output(buf))
	{
		trace("\t\tOCTET_STRING\t%i\t%s\r\n", str->len, str->str);
	}

	return str;
}

int sml_octet_string_write(octet_string *str, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (str == 0)
	{
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	buf = sml_buffer_resize(buf, str->len + SML_BUFFER_ADDITIONAL_LENGTH);
	if (!buf)
	{
		return SML_MALLOC_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_OCTET_STRING, (unsigned int)str->len);
	if (rv != SML_OK) return rv;

	memcpy(sml_buf_get_current_buf(buf), str->str, str->len);
	buf->cursor += str->len;

	return rv;
}

void sml_octet_string_free(octet_string *str)
{
	if (str)
    {
		if (str->str)
        {
			free(str->str);
		}
		free(str);
	}
}

octet_string *sml_octet_string_init_from_hex(char *str)
{
	int i, len = strlen(str);
	unsigned char *bytes = NULL;
	octet_string *o = NULL;

	if (len % 2 != 0)
	{
		return NULL;
	}
	//unsigned char bytes[len / 2];
	bytes = (unsigned char *)malloc(len / 2);
	if (!bytes)
	{
		trace("ERROR: malloc %d bytes\n", len / 2);
		return NULL;
	}

	for (i = 0; i < (len / 2); i++)
	{
		bytes[i] = c2ptoi(&(str[i * 2]));
	}

	o = sml_octet_string_init(bytes, len / 2);

	free(bytes);

	return o;
}

octet_string *sml_octet_string_generate_uuid(void)
{
#ifndef _NO_UUID_LIB
	uuid_t uuid;
	uuid_generate(uuid);
#else
	unsigned char uuid[16];

// TODO add support for WIN32 systems
#ifdef __linux__
	#ifndef USE_POKY_LINUX
		int fd = open("/dev/urandom", O_RDONLY);
		read(fd, uuid, 16);
	#else
		int i;
		for (i = 0; i < 16; i++)
		{
			uuid[i] = rand() % 0xFF;
		}
	#endif
#else
	int i;
	for(i = 0; i < 16; i++)
    {
		uuid[i] = rand() % 0xFF;
	}
#endif /* __linux__ */
	uuid[6] = (uuid[6] & 0x0F) | 0x40; // set version
	uuid[8] = (uuid[8] & 0x3F) | 0x80; // set reserved bits
#endif /* _NO_UUID_LIB */
	return sml_octet_string_init(uuid, 16);
}

int sml_octet_string_cmp(octet_string *s1, octet_string *s2)
{
	if (s1->len != s2->len)
    {
		return -1;
	}
	return memcmp(s1->str, s2->str, s1->len);
}

int sml_octet_string_cmp_with_hex(octet_string *str, char *hex)
{
    int result;
	octet_string *hstr = sml_octet_string_init_from_hex(hex);
	if (str->len != hstr->len)
    {
		sml_octet_string_free(hstr);
		return -1;
	}
	result = memcmp(str->str, hstr->str, str->len);
	sml_octet_string_free(hstr);
	return result;
}

uint8_t ctoi(uint8_t c)
{
	uint8_t ret = 0;

	if((c >= '0') && (c <= '9'))
    {
		ret = c - '0';
	}
	else if((c >= 'a') && (c <= 'f'))
    {
		ret = c - 'a' + 10;
	}
	else if((c >= 'A') && (c <= 'F'))
    {
		ret = c - 'A' + 10;
	}
	return ret;
}

uint8_t c2toi(uint8_t c1, uint8_t c2)
{
	return ctoi(c1) << 4 | ctoi(c2);
}

uint8_t c2ptoi(char* c)
{
	return ctoi((uint8_t)c[0]) << 4 | ctoi((uint8_t)c[1]);
}
