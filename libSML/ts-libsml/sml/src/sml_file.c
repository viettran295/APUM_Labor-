// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <string.h>
#include <sml/sml_file.h>
#include <sml/sml_shared.h>
#include <sml/sml_message.h>
#include <sml/sml_number.h>
#include <sml/sml_time.h>
#include <sml/sml_log.h>

sml_file *sml_file_init(int flags)
{
	return sml_file_init2(SML_BUFFER_LENGTH, flags);
}

sml_file *sml_file_init2(size_t buffer_len, int flags)
{
	sml_buffer *buf = NULL;

	sml_file *file = (sml_file*)malloc(sizeof(sml_file));
	if (!file)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_file));
		return NULL;
	}

	memset(file, 0, sizeof(sml_file));

	buf = sml_buffer_init(buffer_len, flags);
	if (!buf)
	{
		return NULL;
	}

	file->buf = buf;

	return file;
}

sml_file *sml_file_parse(unsigned char *buffer, size_t buffer_len, int flags)
{
    size_t size = buffer_len + SML_BUFFER_ADDITIONAL_LENGTH;
    sml_buffer *buf = NULL;

	if (!buffer || (buffer_len == 0))
	{
		return NULL;
	}

    sml_file *file = sml_file_init2(size, flags);
	if (!file)
    {
		return NULL;
	}
	buf = file->buf;
	memcpy(buf->buffer, buffer, buffer_len);

	// parsing all messages
	for (; buf->cursor < (int)buf->buffer_len;)
    {
        sml_message *msg = NULL;

		if(sml_buf_get_current_byte(buf) == SML_MESSAGE_END)
        {
			// reading trailing zeroed bytes
			sml_buf_update_bytes_read(buf, 1);
			continue;
		}

        msg = sml_message_parse(buf);

        if (buf->error == SML_MESSAGE_CRC_ERROR)
        {
            if (flags & SML_FLAGS_IGNORE_CRC_ERRORS)
            {
		        buf->error = 0;
            }
            else
            {
                if (msg)
                {
                    sml_message_free(msg);
                    break;
                }
            }
        }

		if (sml_buf_has_errors(buf))
        {
			if (sml_debug_output(buf))
            {
				trace("warning: could not read the whole file\n");
			}
			break;
		}

	    sml_file_add_message(file, msg);
	}

	return file;
}

int sml_file_write(sml_file *file)
{
	int rv = SML_OK;
	
	if (!file)
	{
		return SML_PARAMETER_ERROR;
	}

	if (file->messages && file->messages_len > 0)
    {
		int i;
		for (i = 0; i < file->messages_len; i++)
        {
			rv = sml_message_write(file->messages[i], file->buf);
			if (rv != SML_OK) return rv;
		}
	}

	return rv;
}

void sml_file_free(sml_file *file)
{
	if (file)
	{
		if (file->messages)
		{
			int i;
			for (i = 0; i < file->messages_len; i++)
			{
				sml_message_free(file->messages[i]);
			}
			free(file->messages);
		}

		if (file->buf)
		{
			sml_buffer_free(file->buf);
		}

		free(file);
	}
}

int sml_file_add_message(sml_file *file, sml_message *message)
{
	int rv = SML_OK;

	if (!file || !message)
	{
		return SML_PARAMETER_ERROR;
	}

	file->messages_len++;
	file->messages = (sml_message **)realloc(file->messages, sizeof(sml_message *) * file->messages_len);
	if (!file->messages)
	{
		trace("ERROR: realloc %ld bytes\n", sizeof(sml_message *) * file->messages_len);
		file->buf->error = SML_MALLOC_ERROR;
		return SML_MALLOC_ERROR;
	}

	file->messages[file->messages_len - 1] = message;

	return rv;
}

void sml_file_print(sml_file *file)
{
	int i;
	
	trace("SML file (%d SML messages, %d bytes)\n", file->messages_len, file->buf->cursor);
	for (i = 0; i < file->messages_len; i++)
    {
		trace("SML message %4.X\n", *(file->messages[i]->message_body->tag));
	}
}

unsigned char *sml_file_get_buffer(sml_file *file)
{
	unsigned char *buf = NULL;
	if (file->buf)
    {
		buf = file->buf->buffer;
	}
	return buf;
}

int sml_file_get_buffer_len(sml_file *file)
{
	int len = 0;
	if (file->buf)
    {
		len = file->buf->cursor;
	}
	return len;
}

int sml_file_add_flags(sml_file *file, int flags)
{
	int newFlags = 0;
	if (file->buf)
    {
		file->buf->flags |= flags;
		newFlags = file->buf->flags;
	}
	return newFlags;
}

int sml_file_get_error(sml_file *file)
{
	int error = -99;
	if (file->buf)
    {
		error = file->buf->error;
	}
	return error;
}
