// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <time.h>
#include <string.h>

#ifndef USE_HAL_DRIVER
	#include <sys/types.h>
	#ifndef WIN32
		#include <unistd.h>
	#endif
#endif

#include <sml/sml_transport.h>
#include <sml/sml_shared.h>
#include <sml/sml_crc16.h>
#include <sml/sml_log.h>

unsigned char esc_seq_default[] = { 0x1b, 0x1b, 0x1b, 0x1b };
unsigned char start_seq[] = { 0x01, 0x01, 0x01, 0x01 };
unsigned char end_seq[] = { 0x1a };

size_t sml_transport_read(unsigned char *mem, unsigned char *buffer, size_t max_len)
{
    return sml_transport_read2(mem, buffer, max_len, 0, esc_seq_default);
}

size_t sml_transport_read2(unsigned char *mem, unsigned char *buffer, size_t max_len, int flags, unsigned char *esc_seq)
{
	size_t rv = 0;
	unsigned char *buf = NULL;
	unsigned int len_mem = 0;
	unsigned int len_buf = 0;

	if (!mem || !buffer || (max_len < 8) || !esc_seq)
	{
		trace("ERROR: parameter\n");
		return 0;
	}

	buf = (unsigned char *)malloc(max_len);
	if (!buf)
	{
		trace("ERROR: malloc %ld bytes\n", max_len);
		return 0;
	}
	memset(buf, 0, max_len);

	if ((memcmp(&mem[len_mem], esc_seq, 4) == 0) && (memcmp(&mem[len_mem + 4], start_seq, 4) == 0))
	{
		len_mem += 8;
	}
	else
	{
		goto end;
	}

	// found start sequence

	while (len_mem <= max_len - 4)
	{
		if (memcmp(&mem[len_mem], esc_seq, 4) == 0)
		{
			// found esc sequence
			len_mem += 4;

			if (mem[len_mem] == 0x1a)
			{
				u16 crc;
				unsigned char crcBuf[2];

				// found end sequence
				len_mem += 4;

				crc = sml_crc16_calculate(&(mem[0]), len_mem - 2);
				crcBuf[0] = (unsigned char)((crc & 0xFF00) >> 8);
				crcBuf[1] = (unsigned char)(crc & 0x00FF);
				if ((memcmp(crcBuf, &(mem[len_mem - 2]), 2) == 0) ||
					(flags & SML_FLAGS_IGNORE_CRC_ERRORS))
				{
					memcpy(buffer, buf, len_buf);
					rv = len_buf;
					goto end;
				}
				else
				{
					goto end;
				}
			}
			else if (memcmp(&mem[len_mem], esc_seq, 4) == 0)
			{
				memcpy(&buf[len_buf], &mem[len_mem], 4);
				len_buf += 4;
			}
			else
			{
				// dont read other escaped sequences yet
				trace("error: unrecognized sequence\n");
				goto end;
			}
		}
		else
		{
			memcpy(&buf[len_buf], &mem[len_mem], 4);
			len_buf += 4;
		}
		len_mem += 4;
	}

end:
	if (buf)
		free(buf);

	return rv;
}

int sml_transport_write(sml_file *file)
{
    return sml_transport_write2(file, 0, esc_seq_default);
}

int sml_transport_write2(sml_file *file, int flags, unsigned char *esc_seq)
{
	sml_buffer *bufMsg = NULL;
	sml_buffer *bufTmp = NULL;
	size_t pos = 0;
	int padding;
	int rv = SML_OK;
	u16 crc;

	if (!file || !file->buf || !esc_seq)
	{
		return SML_PARAMETER_ERROR;
	}

	bufMsg = file->buf;

	bufMsg->cursor = 0;
	memset(bufMsg->buffer, 0, bufMsg->buffer_len);

	// serialize sml file
	rv = sml_file_write(file);
	if (rv != SML_OK) goto error;
	
	bufTmp = sml_buffer_init(2* bufMsg->cursor + 16, flags);
	if (!bufTmp)
	{
		return SML_MALLOC_ERROR;
	}

	// add start sequence
	memcpy(sml_buf_get_current_buf(bufTmp), esc_seq, 4);
	bufTmp->cursor += 4;
	memcpy(sml_buf_get_current_buf(bufTmp), start_seq, 4);
	bufTmp->cursor += 4;

	// add padding bytes to sml file
	padding = (bufMsg->cursor % 4) ? (4 - bufMsg->cursor % 4) : 0;
	if (padding)
	{
		// write zeroed bytes
		memset(sml_buf_get_current_buf(bufMsg), 0, padding);
		bufMsg->cursor += padding;
	}

	// add message to sml transport file
	if (bufMsg->cursor > 4)
	{
		pos = 0;
		while (pos <= bufMsg->cursor - 4)
		{
			if (memcmp(&bufMsg->buffer[pos], esc_seq, 4) == 0)
			{
				// found esc sequence
				memcpy(sml_buf_get_current_buf(bufTmp), esc_seq, 4);
				bufTmp->cursor += 4;
				memcpy(sml_buf_get_current_buf(bufTmp), &bufMsg->buffer[pos], 4);
				bufTmp->cursor += 4;
			}
			else
			{
				memcpy(sml_buf_get_current_buf(bufTmp), &bufMsg->buffer[pos], 4);
				bufTmp->cursor += 4;
			}
			pos += 4;
		}
	}

	// begin end sequence
	memcpy(sml_buf_get_current_buf(bufTmp), esc_seq, 4);
	bufTmp->cursor += 4;
	memcpy(sml_buf_get_current_buf(bufTmp), end_seq, 1);
	bufTmp->cursor += 1;

	// add padding info
	bufTmp->buffer[bufTmp->cursor++] = (unsigned char)padding;

	// add crc checksum
	crc = sml_crc16_calculate(bufTmp->buffer, bufTmp->cursor);
	bufTmp->buffer[bufTmp->cursor++] = (unsigned char)((crc & 0xFF00) >> 8);
	bufTmp->buffer[bufTmp->cursor++] = (unsigned char)(crc & 0x00FF);

	memcpy(bufMsg->buffer, bufTmp->buffer, bufTmp->cursor);
	bufMsg->cursor = bufTmp->cursor;

error:
	if (bufTmp)
		sml_buffer_free(bufTmp);

	return rv;
}

int sml_transport_write3(sml_buffer *buf, sml_buffer *bufMsg, int flags, unsigned char *esc_seq)
{
    int padding;
    u16 crc;
	size_t pos = 0;

	UNUSED(flags);   // avoid warning: unused parameter

	if (!buf || !bufMsg || !esc_seq)
	{
		return SML_PARAMETER_ERROR;
	}

	buf = sml_buffer_resize(buf, SML_BUFFER_ADDITIONAL_LENGTH);
	if (!buf)
	{
		return SML_MALLOC_ERROR;
	}

	buf->cursor = 0;
	memset(buf->buffer, 0, buf->buffer_len);

	// add start sequence
	memcpy(sml_buf_get_current_buf(buf), esc_seq, 4);
	buf->cursor += 4;
	memcpy(sml_buf_get_current_buf(buf), start_seq, 4);
	buf->cursor += 4;

	// add padding bytes to sml file
	padding = (bufMsg->cursor % 4) ? (4 - bufMsg->cursor % 4) : 0;
	if (padding)
    {
		// write zeroed bytes
		memset(sml_buf_get_current_buf(bufMsg), 0, padding);
		bufMsg->cursor += padding;
	}

	// add message to sml transport file
	pos = 0;
	while (pos <= bufMsg->cursor - 4)
	{
		if (memcmp(&bufMsg->buffer[pos], esc_seq, 4) == 0)
		{
			// found esc sequence
			memcpy(sml_buf_get_current_buf(buf), esc_seq, 4);
			buf->cursor += 4;
			memcpy(sml_buf_get_current_buf(buf), &bufMsg->buffer[pos], 4);
			buf->cursor += 4;
		}
		else
		{
			memcpy(sml_buf_get_current_buf(buf), &bufMsg->buffer[pos], 4);
			buf->cursor += 4;
		}
		pos += 4;
	}

	// begin end sequence
	memcpy(sml_buf_get_current_buf(buf), esc_seq, 4);
	buf->cursor += 4;
	memcpy(sml_buf_get_current_buf(buf), end_seq, 1);
	buf->cursor += 1;

	// add padding info
	buf->buffer[buf->cursor++] = (unsigned char) padding;

	// add crc checksum
	crc = sml_crc16_calculate(buf->buffer, buf->cursor);
	buf->buffer[buf->cursor++] = (unsigned char) ((crc & 0xFF00) >> 8);
	buf->buffer[buf->cursor++] = (unsigned char) (crc & 0x00FF);

	return SML_OK;
}

