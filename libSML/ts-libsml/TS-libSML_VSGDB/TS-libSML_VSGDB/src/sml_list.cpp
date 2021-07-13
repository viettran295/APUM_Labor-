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
#include <sml/sml_list.h>
#include <sml/sml_shared.h>
#include <sml/sml_time.h>
#include <sml/sml_octet_string.h>
#include <sml/sml_status.h>
#include <sml/sml_value.h>
#include <sml/sml_message.h>
#include <sml/sml_log.h>

// sml_sequence;

sml_sequence *sml_sequence_init(void (*elem_free) (void *elem)) {
	sml_sequence *seq = (sml_sequence *)malloc(sizeof(sml_sequence));
	if (seq) {
		memset(seq, 0, sizeof(sml_sequence));
		seq->elem_free = elem_free;
	}
	return seq;
}

sml_sequence *sml_sequence_parse(sml_buffer *buf, void *(*elem_parse) (sml_buffer *buf), void (*elem_free) (void *elem)) {
	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tSEQUENCE\r\n");
	}

	sml_sequence *seq = sml_sequence_init(elem_free);
	if (!seq) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_sequence));
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	int i, len = sml_buf_get_next_length(buf);
	void *p;
	for (i = 0; i < len; i++) {
		p = elem_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		int rv = sml_sequence_add(seq, p);
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

void sml_sequence_write(sml_sequence *seq, sml_buffer *buf, void (*elem_write) (void *elem, sml_buffer *buf)) {
	if (seq == 0) {
		sml_buf_optional_write(buf);
		return;
	}

	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, seq->elems_len);

	int i;
	for (i = 0; i < seq->elems_len; i++) {
		elem_write((seq->elems)[i], buf);
	}
}

void sml_sequence_free(sml_sequence *seq) {
	if (seq) {
		int i;
		for (i = 0; i < seq->elems_len; i++) {
			seq->elem_free((seq->elems)[i]);
		}
		
		if (seq->elems != 0) {
			free(seq->elems);
		}
		
		free(seq);
	}
}

int sml_sequence_add(sml_sequence *seq, void *new_entry) {
	seq->elems_len++;
	seq->elems = (void **)realloc(seq->elems, sizeof(void *) * seq->elems_len);
	if (!seq->elems) {
		trace("ERROR: malloc %ld bytes\n", sizeof(void *) * seq->elems_len);
		return SML_MALLOC_ERROR;
	}

	seq->elems[seq->elems_len - 1] = new_entry;

	return SML_OK;
}


// sml_list;

sml_list *sml_list_init() {
	sml_list *l = (sml_list *)malloc(sizeof(sml_list));
	if (l) {
		memset(l, 0, sizeof(sml_list));
	}
	return l;
}

void sml_list_add(sml_list *list, sml_list *new_entry) {
	list->next = new_entry;
}

sml_list *sml_list_entry_parse(sml_buffer *buf) {
	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	if (sml_buf_get_next_length(buf) != 7) {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		return NULL;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tLIST ENTRY\r\n");
	}

	sml_list *l = sml_list_init();
	if (!l) {
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_list));
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

sml_list *sml_list_parse(sml_buffer *buf) {
	if (sml_buf_optional_is_skipped(buf)) {
		return 0;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST) {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return 0;
	}

	if (sml_debug_output(buf)) {
		trace("\t\tLIST\r\n");
	}

	sml_list *first = 0;
	sml_list *last = 0;
	int elems;

	elems = sml_buf_get_next_length(buf);

	if (elems > 0) {
		first = sml_list_entry_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		last = first;
		elems--;
	}

	while(elems > 0) {
		last->next = sml_list_entry_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		last = last->next;
		elems--;
	}

	return first;

error:
	sml_list_free(first);
	return 0;
}


void sml_list_entry_write(sml_list *list, sml_buffer *buf) {
	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 7);
	sml_octet_string_write(list->obj_name, buf);
	sml_status_write(list->status, buf);
	sml_time_write(list->val_time, buf);
	sml_u8_write(list->unit, buf);
	sml_i8_write(list->scaler, buf);
	sml_value_write(list->value, buf);
	sml_octet_string_write(list->value_signature, buf);
}

void sml_list_write(sml_list *list, sml_buffer *buf) {
	if (list == 0) {
		sml_buf_optional_write(buf);
		return;
	}

	sml_list *i = list;
	int len = 0;
	while(i) {
		i = i->next;
		len++;
	}

	sml_buf_set_type_and_length(buf, SML_TYPE_LIST, len);

	i = list;
	while(i) {
		sml_list_entry_write(i, buf);
		i = i->next;
	}
}

void sml_list_entry_free(sml_list *list) {
	if (list) {
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

void sml_list_free(sml_list *list) {
	if (list) {
		sml_list *f = list;
		sml_list *n = list->next;

		while(f) {
			sml_list_entry_free(f);
			f = n;
			if (f) {
				n = f->next;
			}
		}
	}
}

int build_signed_content_edl(sml_list *list, unsigned char *server_id, size_t server_id_len, unsigned char *buf, size_t len)
{
	int pos = 0;

	if (!list || !buf || (len != 48))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38
	memset(buf + pos, 0, len);

	// device identification (0 ... 9)
	if (!server_id || server_id_len > 10)
		return 0;
	memcpy(buf + pos, server_id, server_id_len);
	pos += 10;

	// timestamp  (10 ... 13)
	if (!list->val_time || (*(list->val_time->tag) != SML_TIME_SEC_INDEX))
		return 0;
	u32 secIndex = *(list->val_time->data.sec_index);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (14)
	if (!list->status)
		return 0;
	u32 status = *(list->status->data.status32);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// OBIS word (15 ... 20)
	if (!list->obj_name || (list->obj_name->len != 6))
		return 0;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (21)
	if (!list->unit)
		return 0;
	u8 unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (22)
	if (!list->scaler)
		return 0;
	i8 scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (23 ... 30)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return 0;
	u64 value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	return 1;
}

int build_signed_content_bz_slp(sml_list *list, unsigned char *server_id, size_t server_id_len, unsigned char *buf, size_t len)
{
	int pos = 0;

	if (!list || !buf || (len != 96))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
	memset(buf + pos, 0, len);

	// device identification (0 ... 13)
	if (!server_id || server_id_len > 14)
		return 0;
	memcpy(buf + pos, server_id, server_id_len);
	pos += 14;

	// timestamp  (14 ... 17)
	if (!list->val_time || (*(list->val_time->tag) != SML_TIME_SEC_INDEX))
		return 0;
	u32 secIndex = *(list->val_time->data.sec_index);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (18 ... 21)
	if (!list->status)
		return 0;
	u32 status = *(list->status->data.status32);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// OBIS word (22 ... 27)
	if (!list->obj_name || (list->obj_name->len != 6))
		return 0;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (28)
	if (!list->unit)
		return 0;
	u8 unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (29)
	if (!list->scaler)
		return 0;
	i8 scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return 0;
	u64 value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	return 1;
}

int build_signed_content_bz_rlm(sml_list *list, unsigned char *server_id, size_t server_id_len, unsigned char *buf, size_t len)
{
	int pos = 0;

	if (!list || !buf || (len != 96))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
	memset(buf + pos, 0, len);

	// device identification (0 ... 13)
	if (!server_id || server_id_len > 14)
		return 0;
	memcpy(buf + pos, server_id, server_id_len);
	pos += 14;

	// timestamp  (14 ... 17)
	if (!list->val_time || (*(list->val_time->tag) != SML_TIME_SEC_INDEX))
		return 0;
	u32 secIndex = *(list->val_time->data.sec_index);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (18 ... 21)
	if (!list->status)
		return 0;
	u32 status = *(list->status->data.status32);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// TODO: clarify where to find '+A' values and where to store the signature at the end
	// OBIS word '+A' (22 ... 27)
	if (!list->obj_name || (list->obj_name->len != 6))
		return 0;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (28)
	if (!list->unit)
		return 0;
	u8 unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (29)
	if (!list->scaler)
		return 0;
	i8 scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return 0;
	u64 value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	// TODO: clarify where to find 'R1' values and where to store the signature at the end
	// OBIS word 'R1' (38 ... 43)
	if (!list->obj_name || (list->obj_name->len != 6))
		return 0;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (44)
	if (!list->unit)
		return 0;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (45)
	if (!list->scaler)
		return 0;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (46 ... 53)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return 0;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	// TODO: clarify where to find 'R4' values and where to store the signature at the end
	// OBIS word 'R4' (54 ... 59)
	if (!list->obj_name || (list->obj_name->len != 6))
		return 0;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (60)
	if (!list->unit)
		return 0;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (61)
	if (!list->scaler)
		return 0;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (62 ... 69)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return 0;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	return 1;
}

void sml_list_entry_build_signed_content(int type, sml_list *list, unsigned char *server_id, size_t server_id_len, unsigned char *buf, size_t len)
{
	if (!list || (!server_id) || (server_id_len < 1) || (!buf))
		return;

	if (type == SML_MESSAGE_TYPE_EDL)
	{
		if ((server_id_len > 10) || (len != 48))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38
		build_signed_content_edl(list, server_id, server_id_len, buf, len);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_SLP) 
	{
		if ((server_id_len > 14) || (len != 96))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
		build_signed_content_bz_slp(list, server_id, server_id_len, buf, len);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_RLM) 
	{
		if ((server_id_len > 14) || (len != 96))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
		build_signed_content_bz_rlm(list, server_id, server_id_len, buf, len);
	}
	else
	{
		return;
	}

	return;
}
