// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_tree.h>
#include <sml/sml_value.h>
#include <sml/sml_number.h>
#include <sml/sml_log.h>

// sml_tree_path;

sml_tree_path *sml_tree_path_init()
{
	sml_tree_path *tree_path = (sml_tree_path *)malloc(sizeof(sml_tree_path));
	if (!tree_path)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_tree_path));
		return NULL;
	}
	memset(tree_path, 0, sizeof(sml_tree_path));
	
	return tree_path;
}

sml_tree_path *sml_tree_path_parse(sml_buffer *buf)
{
    sml_tree_path *tree_path = NULL;
    octet_string *s = NULL;
    int elems;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	tree_path = sml_tree_path_init();
	if (!tree_path)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		return NULL;
	}

	elems = sml_buf_get_next_length(buf);

	if (sml_debug_output(buf))
    {
		trace("\t\tTREE PATH\t%i\r\n", elems);
	}

	for (elems; elems > 0; elems--)
    {
		s = sml_octet_string_parse(buf);
		if (sml_buf_has_errors(buf)) goto error;
		if (s)
        {
			int rv = sml_tree_path_add_path_entry(tree_path, s);
			if (rv == SML_MALLOC_ERROR)
			{
				buf->error = SML_MALLOC_ERROR;
				goto error;
			}
		}
	}

	return tree_path;

error:
	sml_tree_path_free(tree_path);
	return NULL;
}

int sml_tree_path_write(sml_tree_path *tree_path, sml_buffer *buf)
{
	int rv = SML_OK;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (tree_path == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	if (tree_path->path_entries && tree_path->path_entries_len > 0)
    {
        int i;

        rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, tree_path->path_entries_len);
		if (rv != SML_OK) return rv;
		
		for (i = 0; i < tree_path->path_entries_len; i++)
        {
			rv = sml_octet_string_write(tree_path->path_entries[i], buf);
			if (rv != SML_OK) return rv;
		}
	}

	return rv;
}

void sml_tree_path_free(sml_tree_path *tree_path)
{
	if (tree_path)
    {
		if (tree_path->path_entries && tree_path->path_entries_len > 0)
        {
			int i;
			for (i = 0; i < tree_path->path_entries_len; i++)
            {
				sml_octet_string_free(tree_path->path_entries[i]);
			}

			free(tree_path->path_entries);
		}

		free(tree_path);
	}
}

int sml_tree_path_add_path_entry(sml_tree_path *tree_path, octet_string *entry)
{
	tree_path->path_entries_len++;
	tree_path->path_entries = (octet_string **)realloc(tree_path->path_entries, sizeof(octet_string *) * tree_path->path_entries_len);
	if (!tree_path->path_entries)
	{
		trace("ERROR: realloc %ld bytes\n", sizeof(octet_string *) * tree_path->path_entries_len);
		return SML_MALLOC_ERROR;
	}

	tree_path->path_entries[tree_path->path_entries_len - 1] = entry;

	return SML_OK;
}

// sml_tree;

sml_tree *sml_tree_init(void)
{
	sml_tree *tree = (sml_tree *)malloc(sizeof(sml_tree));
	if (!tree)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_tree));
		return NULL;
	}
	memset(tree, 0, sizeof(sml_tree));
	
	return tree;
}

sml_tree *sml_tree_parse(sml_buffer *buf)
{
    sml_tree *tree = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	tree = sml_tree_init();
	if (!tree)
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
		trace("\t\tTREE\r\n");
	}

	tree->parameter_name = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tree->parameter_value = sml_proc_par_value_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	if (!sml_buf_optional_is_skipped(buf))
    {
        sml_tree *c = NULL;
        int elems;
        
        if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
        {
			buf->error = SML_TYPE_LIST_EXPECTED;
			goto error;
		}

		elems = sml_buf_get_next_length(buf);

		if (sml_debug_output(buf))
        {
			trace("\t\tTREE child_list\t%i\r\n", elems);
		}

		for (elems; elems > 0; elems--)
        {
			c = sml_tree_parse(buf);
			if (sml_buf_has_errors(buf)) goto error;
			if (c)
            {
				int rv = sml_tree_add_tree(tree, c);
				if (rv == SML_MALLOC_ERROR)
				{
					buf->error = SML_MALLOC_ERROR;
					goto error;
				}
			}
		}
	}

	return tree;

error:
	sml_tree_free(tree);
	return NULL;
}

int sml_tree_write(sml_tree *tree, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (tree == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 3);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(tree->parameter_name, buf);
	if (rv != SML_OK) return rv;

	rv = sml_proc_par_value_write(tree->parameter_value, buf);
	if (rv != SML_OK) return rv;

	if (tree->child_list && tree->child_list_len > 0)
    {
        int i;
        
        rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, tree->child_list_len);
		if (rv != SML_OK) return rv;

		for (i = 0; i < tree->child_list_len; i++)
        {
			rv = sml_tree_write(tree->child_list[i], buf);
			if (rv != SML_OK) return rv;
		}
	}
	else
    {
		rv = sml_buf_optional_write(buf);
		if (rv != SML_OK) return rv;
	}

	return rv;
}

void sml_tree_free(sml_tree *tree)
{
	if (tree)
    {
        int i;
        
        sml_octet_string_free(tree->parameter_name);
		sml_proc_par_value_free(tree->parameter_value);

		for (i = 0; i < tree->child_list_len; i++)
        {
			sml_tree_free(tree->child_list[i]);
		}

		free(tree->child_list);
		free(tree);
	}
}

int sml_tree_add_tree(sml_tree *base_tree, sml_tree *tree)
{
	base_tree->child_list_len++;
	base_tree->child_list = (sml_tree **)realloc(base_tree->child_list, sizeof(sml_tree *) * base_tree->child_list_len);
	if (!base_tree->child_list)
	{
		trace("ERROR: realloc %ld bytes\n", sizeof(sml_tree *) * base_tree->child_list_len);
		return SML_MALLOC_ERROR;
	}
	base_tree->child_list[base_tree->child_list_len - 1] = tree;

	return SML_OK;
}

// sml_proc_par_value;

sml_proc_par_value *sml_proc_par_value_init(void)
{
	sml_proc_par_value *value = (sml_proc_par_value *)malloc(sizeof(sml_proc_par_value));
	if (!value)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_proc_par_value));
		return NULL;
	}
	memset(value, 0, sizeof(sml_proc_par_value));
	
	return value;
}

sml_proc_par_value *sml_proc_par_value_parse(sml_buffer *buf)
{
    sml_proc_par_value *ppv = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	ppv = sml_proc_par_value_init();
	if (!ppv)
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

	ppv->tag = sml_u8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	if (!ppv->tag) goto error;

	if (sml_debug_output(buf))
    {
		trace("\t\tPROC PAR VALUE\t%i\r\n", *(ppv->tag));
	}

	switch (*(ppv->tag))
    {
		case SML_PROC_PAR_VALUE_TAG_VALUE:
			ppv->data.value = sml_value_parse(buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_PERIOD_ENTRY:
			ppv->data.period_entry = sml_period_entry_parse(buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_TUPEL_ENTRY:
			ppv->data.tupel_entry = sml_tupel_entry_parse(buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_TIME:
			ppv->data.time = sml_time_parse(buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_LIST_ENTRY:
			ppv->data.list_entry = sml_list_entry_parse(buf);
			break;
		default:
			buf->error = SML_PROC_PAR_VALUE_TAG_MISMATCH;
			goto error;
	}

	return ppv;

error:
	sml_proc_par_value_free(ppv);
	return NULL;
}

int sml_proc_par_value_write(sml_proc_par_value *value, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (value == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 2);
	if (rv != SML_OK) return rv;

	rv = sml_u8_write(value->tag, buf);
	if (rv != SML_OK) return rv;

	switch (*(value->tag))
    {
		case SML_PROC_PAR_VALUE_TAG_VALUE:
			rv = sml_value_write(value->data.value, buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_PERIOD_ENTRY:
			rv = sml_period_entry_write(value->data.period_entry, buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_TUPEL_ENTRY:
			rv = sml_tupel_entry_write(value->data.tupel_entry, buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_TIME:
			rv = sml_time_write(value->data.time, buf);
			break;
		case SML_PROC_PAR_VALUE_TAG_LIST_ENTRY:
			rv = sml_list_entry_write(value->data.list_entry, buf);
			break;
		default:
			buf->error = SML_PROC_PAR_VALUE_TAG_MISMATCH;
			if (sml_debug_output(buf))
            {
				trace("error: unknown tag in %s\n", __FUNCTION__);
			}
	}

	return rv;
}

void sml_proc_par_value_free(sml_proc_par_value *ppv)
{
	if (ppv)
    {
		if (ppv->tag)
        {
			switch (*(ppv->tag))
            {
				case SML_PROC_PAR_VALUE_TAG_VALUE:
					sml_value_free(ppv->data.value);
					break;
				case SML_PROC_PAR_VALUE_TAG_PERIOD_ENTRY:
					sml_period_entry_free(ppv->data.period_entry);
					break;
				case SML_PROC_PAR_VALUE_TAG_TUPEL_ENTRY:
					sml_tupel_entry_free(ppv->data.tupel_entry);
					break;
				case SML_PROC_PAR_VALUE_TAG_TIME:
					sml_time_free(ppv->data.time);
					break;
				case SML_PROC_PAR_VALUE_TAG_LIST_ENTRY:
					sml_list_entry_free(ppv->data.list_entry);
					break;
				default:
					if (ppv->data.value)
                    {
						free(ppv->data.value);
					}
			}
			sml_number_free(ppv->tag);
		}
		else {
			// Without the tag, there might be a memory leak.
			if (ppv->data.value)
            {
				free(ppv->data.value);
			}
		}

		free(ppv);
	}
}


// sml_tuple_entry;

sml_tupel_entry *sml_tupel_entry_init(void)
{
	sml_tupel_entry *tupel = (sml_tupel_entry *)malloc(sizeof(sml_tupel_entry));
	if (!tupel)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_tupel_entry));
		return NULL;
	}
	memset(tupel, 0, sizeof(sml_tupel_entry));
	
	return tupel;
}

sml_tupel_entry *sml_tupel_entry_parse(sml_buffer *buf)
{
    sml_tupel_entry *tupel = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	tupel = sml_tupel_entry_init();
	if (!tupel)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 23)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tTUPEL ENTRY\r\n");
	}

	tupel->server_id = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->sec_index = sml_time_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->status = sml_u64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_pA = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_pA = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_pA = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_R1 = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_R1 = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_R1 = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_R4 = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_R4 = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_R4 = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->signature_pA_R1_R4 = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_mA = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_mA = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_mA = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_R2 = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_R2 = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_R2 = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->unit_R3 = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->scaler_R3 = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;
	tupel->value_R3 = sml_i64_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	tupel->signature_mA_R2_R3 = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return tupel;

error:
	sml_tupel_entry_free(tupel);
	return NULL;
}

int sml_tupel_entry_write(sml_tupel_entry *tupel, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (tupel == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 23);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(tupel->server_id, buf);
	if (rv != SML_OK) return rv;

	rv = sml_time_write(tupel->sec_index, buf);
	if (rv != SML_OK) return rv;

	rv = sml_u64_write(tupel->status, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_pA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_pA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_pA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_R1, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_R1, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_R1, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_R4, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_R4, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_R4, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(tupel->signature_pA_R1_R4, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_mA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_mA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_mA, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_R2, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_R2, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_R2, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(tupel->unit_R3, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(tupel->scaler_R3, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i64_write(tupel->value_R3, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(tupel->signature_mA_R2_R3, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_tupel_entry_free(sml_tupel_entry *tupel)
{
	if (tupel)
    {
		sml_octet_string_free(tupel->server_id);
		sml_time_free(tupel->sec_index);
		sml_number_free(tupel->status);

		sml_unit_free(tupel->unit_pA);
		sml_number_free(tupel->scaler_pA);
		sml_number_free(tupel->value_pA);

		sml_unit_free(tupel->unit_R1);
		sml_number_free(tupel->scaler_R1);
		sml_number_free(tupel->value_R1);

		sml_unit_free(tupel->unit_R4);
		sml_number_free(tupel->scaler_R4);
		sml_number_free(tupel->value_R4);

		sml_octet_string_free(tupel->signature_pA_R1_R4);

		sml_unit_free(tupel->unit_mA);
		sml_number_free(tupel->scaler_mA);
		sml_number_free(tupel->value_mA);

		sml_unit_free(tupel->unit_R2);
		sml_number_free(tupel->scaler_R2);
		sml_number_free(tupel->value_R2);

		sml_unit_free(tupel->unit_R3);
		sml_number_free(tupel->scaler_R3);
		sml_number_free(tupel->value_R3);

		sml_octet_string_free(tupel->signature_mA_R2_R3);

		free(tupel);
	}
}

int sml_tupel_entry_build_signed_content(sml_tupel_entry *tupel, unsigned char *buf_pA, size_t len_pA, unsigned char *buf_mA, size_t len_mA)
{
	int pos = 0;
    u32 secIndex;
    u16 status;
    i8 scalerPA;
    i8 scalerMA;
    i64 valuePA;
    i64 valueMA;
    i64 valueR1;
    i64 valueR2;
    i64 valueR3;
    i64 valueR4;
	unsigned char baPA[8];
	unsigned char baMA[8];
	unsigned char baR1[8];
	unsigned char baR2[8];
	unsigned char baR3[8];
	unsigned char baR4[8];

	if (!tupel || !buf_pA || !buf_mA || (len_pA != 24)|| (len_mA != 24))
		return 0;

	// calculation of hash code (input for signature algo) according Lastenheft SYM2 starting on page 36

	// device identification (0 ... 5)
	// 00 0f 93 02 4a df
	if (!tupel->server_id || tupel->server_id->len != 6)
		return 0;
	memcpy(buf_pA + pos, tupel->server_id->str, tupel->server_id->len);
	memcpy(buf_mA + pos, tupel->server_id->str, tupel->server_id->len);
	pos += tupel->server_id->len;

	// second index (6 ... 9)
	// 00 2e ed 12
	if (!tupel->sec_index || (*(tupel->sec_index->tag) != SML_TIME_SEC_INDEX))
		return 0;
	secIndex = *(tupel->sec_index->data.sec_index);
	if (!(sml_number_endian() == SML_BIG_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf_pA + pos, &secIndex, sizeof(secIndex));
	memcpy(buf_mA + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (10 .. 11)
	// 68 04
	if (!tupel->status)
		return 0;
	status = (u16)*(tupel->status) >> 8;
	if (!(sml_number_endian() == SML_BIG_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf_pA + pos, &status, sizeof(status));
	memcpy(buf_mA + pos, &status, sizeof(status));
	pos += sizeof(status);

	// scale (12)
	// 3f / 7F
	if (!tupel->scaler_pA || !tupel->scaler_mA)
		return 0;
	scalerPA = *(tupel->scaler_pA);
	scalerMA = *(tupel->scaler_mA);
	scalerPA = scalerPA & 0x3F;
	scalerMA = scalerMA & 0x7F;
	buf_pA[pos] = scalerPA;
	buf_mA[pos] = scalerMA;
	pos++;

	// temporary help meter count (13 ... 23)
	// 00 00 97 cc c0 00 0f b8 00 00 19 / 00 02 a6 ad c0 00 15 30 00 00 29
	valuePA = *(tupel->value_pA);
	valueMA = *(tupel->value_mA);
	valueR1 = *(tupel->value_R1);
	valueR2 = *(tupel->value_R2);
	valueR3 = *(tupel->value_R3);
	valueR4 = *(tupel->value_R4);
	valueR1 /= 100;
	valueR2 /= 100;
	valueR3 /= 100;
	valueR4 /= 100;
	valuePA = valuePA << 30;
	valueMA = valueMA << 30;
	valueR1 = valueR1 << (27 + 8);
	valueR2 = valueR2 << (27 + 8);
	valueR3 = valueR3 << (0 + 32);
	valueR4 = valueR4 << (0 + 32);
	if (!(sml_number_endian() == SML_BIG_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&valuePA, sizeof(valuePA));
		sml_number_byte_swap((unsigned char*)&valueMA, sizeof(valueMA));
		sml_number_byte_swap((unsigned char*)&valueR1, sizeof(valueR1));
		sml_number_byte_swap((unsigned char*)&valueR2, sizeof(valueR2));
		sml_number_byte_swap((unsigned char*)&valueR3, sizeof(valueR3));
		sml_number_byte_swap((unsigned char*)&valueR4, sizeof(valueR4));
	}
	memcpy(baPA, &valuePA, sizeof(valuePA));
	memcpy(baMA, &valueMA, sizeof(valueMA));
	memcpy(baR1, &valueR1, sizeof(valueR1));
	memcpy(baR2, &valueR2, sizeof(valueR2));
	memcpy(baR3, &valueR3, sizeof(valueR3));
	memcpy(baR4, &valueR4, sizeof(valueR4));

	memcpy(buf_pA + pos, baPA, 4);
	memcpy(buf_mA + pos, baMA, 4);
	pos += 4;
	buf_pA[pos] = baPA[4] | baR1[0];
	buf_mA[pos] = baMA[4] | baR2[0];
	pos++;
	memcpy(buf_pA + pos, baR1 + 1, 2);
	memcpy(buf_mA + pos, baR2 + 1, 2);
	pos += 2;
	buf_pA[pos] = baR1[3] | baR4[0];
	buf_mA[pos] = baR2[3] | baR3[0];
	pos++;
	memcpy(buf_pA + pos, baR4 + 1, 3);
	memcpy(buf_mA + pos, baR3 + 1, 3);
	pos += 3;

	return 1;
}


// sml_period_entry;

sml_period_entry *sml_period_entry_init(void)
{
	sml_period_entry *period = (sml_period_entry *)malloc(sizeof(sml_period_entry));
	if (!period)
	{
		trace("ERROR: malloc %ld bytes\n", sizeof(sml_period_entry));
		return NULL;
	}
	memset(period, 0, sizeof(sml_period_entry));
	
	return period;
}

sml_period_entry *sml_period_entry_parse(sml_buffer *buf)
{
    sml_period_entry *period = NULL;

	if (!buf)
	{
		return NULL;
	}

	if (sml_buf_optional_is_skipped(buf))
    {
		return NULL;
	}

	period = sml_period_entry_init();
	if (!period)
    {
		buf->error = SML_MALLOC_ERROR;
		return NULL;
	}

	if (sml_buf_get_next_type(buf) != SML_TYPE_LIST)
    {
		buf->error = SML_TYPE_LIST_EXPECTED;
		goto error;
	}

	if (sml_buf_get_next_length(buf) != 5)
    {
		buf->error = SML_EXPECTED_LENGTH_NOT_FOUND;
		goto error;
	}

	if (sml_debug_output(buf))
    {
		trace("\t\tPERIOD ENTRY\r\n");
	}

	period->obj_name = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	period->unit = sml_unit_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	period->scaler = sml_i8_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	period->value = sml_value_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	period->value_signature = sml_octet_string_parse(buf);
	if (sml_buf_has_errors(buf)) goto error;

	return period;

error:
	sml_period_entry_free(period);
	return NULL;
}

int sml_period_entry_write(sml_period_entry *period, sml_buffer *buf)
{
	int rv;

	if (!buf)
	{
		return SML_PARAMETER_ERROR;
	}

	if (period == 0)
    {
		rv = sml_buf_optional_write(buf);
		return rv;
	}

	rv = sml_buf_set_type_and_length(buf, SML_TYPE_LIST, 5);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(period->obj_name, buf);
	if (rv != SML_OK) return rv;

	rv = sml_unit_write(period->unit, buf);
	if (rv != SML_OK) return rv;

	rv = sml_i8_write(period->scaler, buf);
	if (rv != SML_OK) return rv;

	rv = sml_value_write(period->value, buf);
	if (rv != SML_OK) return rv;

	rv = sml_octet_string_write(period->value_signature, buf);
	if (rv != SML_OK) return rv;

	return rv;
}

void sml_period_entry_free(sml_period_entry *period)
{
	if (period)
    {
		sml_octet_string_free(period->obj_name);
		sml_unit_free(period->unit);
		sml_number_free(period->scaler);
		sml_value_free(period->value);
		sml_octet_string_free(period->value_signature);

		free(period);
	}
}
