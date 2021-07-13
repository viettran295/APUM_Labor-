// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_list.h>
#include <sml/sml_shared.h>
#include <sml/sml_time.h>
#include <sml/sml_octet_string.h>
#include <sml/sml_status.h>
#include <sml/sml_value.h>
#include <sml/sml_message.h>
#include <sml/sml_log.h>

// sml_sequence;

sml_sequence *sml_sequence_init(void (*elem_free) (void *elem))
{
	sml_sequence *seq = (sml_sequence *)malloc(sizeof(sml_sequence));
	if (!seq)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_sequence));
		return NULL;
	}
	memset(seq, 0, sizeof(sml_sequence));
	seq->elem_free = elem_free;
	
	return seq;
}

sml_sequence *sml_sequence_parse(sml_buffer *buf, void *(*elem_parse) (sml_buffer *buf), void (*elem_free) (void *elem))
{
	if (!buf || !elem_parse || !elem_free)
	{
		return NULL;
	}

    sml_sequence *seq = NULL;
	int i, len = sml_buf_get_next_length(buf);

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tSEQUENCE\r\n");
	}

	seq = sml_sequence_init(elem_free);
	if (!seq)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	for (i = 0; i < len; i++)
    {
        void *p = NULL;
        int rv;

		p = elem_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		rv = sml_sequence_add(seq, p);
		if (rv == SML_MALLOC_ERROR)
		{
			buf->error = SML_MALLOC_ERROR;
			goto error;
		}
	}

	return seq;

error:
	sml_sequence_free(seq);
	return NULL;
}

int sml_sequence_write(sml_sequence *seq, sml_buffer *buf, int (*elem_write) (void *elem, sml_buffer *buf))
{
    int i;
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

    if (seq == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, seq->elems_len);
	if (rv != SML_OK) return rv;

	for (i = 0; i < seq->elems_len; i++)
    {
		rv = elem_write((seq->elems)[i], buf);
		if (rv != SML_OK) return rv;
	}

	return rv;
}

void sml_sequence_free(sml_sequence *seq)
{
	if (seq)
    {
		int i;
		for (i = 0; i < seq->elems_len; i++)
        {
			seq->elem_free((seq->elems)[i]);
		}
		
		if (seq->elems != 0)
        {
			free(seq->elems);
		}
		
		free(seq);
	}
}

int sml_sequence_add(sml_sequence *seq, void *new_entry)
{
	int rv = SML_OK;

	if (!seq || !new_entry)
	{
		return SML_PARAMETER_ERROR;
	}

	seq->elems_len++;
	seq->elems = (void **)realloc(seq->elems, sizeof(void *) * seq->elems_len);
	if (!seq->elems)
    {
		trace("ERROR: realloc %ld bytes\n", sizeof(void *) * seq->elems_len);
		return SML_MALLOC_ERROR;
	}

	seq->elems[seq->elems_len - 1] = new_entry;

	return rv;
}


// sml_list;

sml_list *sml_list_init(void)
{
	sml_list *l = (sml_list *)malloc(sizeof(sml_list));
	if (!l)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_list));
		return NULL;
	}
	memset(l, 0, sizeof(sml_list));
	
	return l;
}

void sml_list_add(sml_list *list, sml_list *new_entry)
{
	list->next = new_entry;
}

sml_list *sml_list_entry_parse(sml_buffer *buf)
{
    sml_list *l = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	if (sml_buf_get_next_length(buf) != 7)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		return NULL;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tLIST ENTRY\r\n");
	}

	l = sml_list_init();
	if (!l)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	l->obj_name = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->status = sml_status_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->val_time = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->unit = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->scaler = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->value = sml_value_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	l->value_signature = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return l;

// This function doesn't free the allocated memory in error cases,
// this is done in sml_list_parse.
error:
	return NULL;
}

sml_list *sml_list_parse(sml_buffer *buf)
{
    sml_list *first = 0;
	sml_list *last = 0;
	int elems;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tLIST\r\n");
	}

	elems = sml_buf_get_next_length(buf);

	if (elems > 0)
    {
		first = sml_list_entry_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		last = first;
		elems--;
	}

	while(elems > 0)
    {
		last->next = sml_list_entry_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		last = last->next;
		elems--;
	}

	return first;

error:
	sml_list_free(first);
	return NULL;
}


int sml_list_entry_write(sml_list *list, sml_buffer *buf)
{
	int rv;

	if (!list || !buf)
	{
		return SML_PARAMETER_ERROR;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 7);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(list->obj_name, buf);
	if (rv != SML_OK) return rv;

	rv = sml_status_write(list->status, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(list->val_time, buf);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(list->unit, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(list->scaler, buf);
	if (rv != SML_OK) return rv;

	rv = sml_value_write(list->value, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(list->value_signature, buf);

	return rv;
}

int sml_list_write(sml_list *list, sml_buffer *buf)
{
    sml_list *i = list;
	int len = 0;
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

    if (list == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	while(i)
    {
		i = i->next;
		len++;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, len);
	if (rv != SML_OK) return rv;

	i = list;
	while(i)
    {
		rv = sml_list_entry_write(i, buf);
		if (rv != SML_OK) return rv;

		i = i->next;
	}

	return rv;
}

void sml_list_entry_free(sml_list *list)
{
	if (list)
    {
		sml_octet_string_free(list->obj_name);
		sml_status_free(list->status);
		sml_time_free(list->val_time);
		sml_number_free(list->unit);
		sml_number_free(list->scaler);
		sml_value_free(list->value);
		sml_octet_string_free(list->value_signature);
		
		free(list);
	}
}

void sml_list_free(sml_list *list)
{
	if (list)
    {
		sml_list *f = list;
		sml_list *n = list->next;

		while(f)
        {
			sml_list_entry_free(f);
			f = n;
			if (f)
            {
				n = f->next;
			}
		}
	}
}
