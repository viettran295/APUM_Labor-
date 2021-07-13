// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_signature.h>
#include <sml/sml_message.h>

octet_string *sml_list_build_signed_content_edl(octet_string *server_id, sml_list *list, octet_string *log_book)
{
	unsigned char buf[48] = {0};
	int pos = 0;
	octet_string *signedContent = NULL;
    sml_time *smlTime = NULL;
    u32 secIndex = 0;
    sml_status *smlStatus = NULL;
    u8 status;
    u8 unit;
    i8 scaler;
    u64 value;

	if (!server_id || !list || !log_book)
		return NULL;

	// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38

	// device identification (0 ... 9)
	if (server_id->len > 10)
		return NULL;
	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 10;

	// timestamp  (10 ... 13)
	smlTime = list->val_time;
	if (!smlTime)
		return NULL;
	switch (*(smlTime->tag))
	{
		case SML_TIME_SEC_INDEX:
			secIndex = *smlTime->data.sec_index;
			break;
		case SML_TIME_TIMESTAMP:
			secIndex = *smlTime->data.timestamp;
			break;
		case SML_TIME_TIMESTAMP_LOCAL:	// TODO: should not be present!!!
			secIndex = *smlTime->data.timestamp_local->timestamp;
			break;
			//return NULL;
		default:
			return NULL;
	}
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (14)
	smlStatus = list->status;
	if (!smlStatus || (smlStatus->type != 0x61))
		return NULL;
	status = *(smlStatus->data.status8);
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// OBIS word (15 ... 20)
	if (!list->obj_name || (list->obj_name->len != 6))
		return NULL;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (21)
	if (!list->unit)
		return NULL;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (22)
	if (!list->scaler)
		return NULL;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (23 ... 30)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return NULL;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	// log book (31 ... 32)
	if (!log_book || (log_book->len != 2))
		return NULL;
	memcpy(buf + pos, log_book->str, log_book->len);
	pos += log_book->len;

	signedContent = sml_octet_string_init(buf, sizeof(buf));

	return signedContent;
}

octet_string *sml_list_build_signed_content_bz_slp(octet_string *server_id, sml_list *list)
{
	unsigned char buf[96] = {0};
	int pos = 0;
	octet_string *signedContent = NULL;
    sml_time *smlTime = NULL;
	u32 secIndex = 0;
    u32 status;
    u8 unit;
    i8 scaler;
    u64 value;

	if (!server_id || !list)
		return NULL;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56

	// device identification (0 ... 13)
	if (server_id->len >= 14)
		return NULL;

	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 14;

	// timestamp  (14 ... 17)
	smlTime = list->val_time;
	if (!smlTime)
		return NULL;
	switch (*(smlTime->tag))
	{
		case SML_TIME_SEC_INDEX:
			secIndex = *smlTime->data.sec_index;
			break;
		case SML_TIME_TIMESTAMP:
			secIndex = *smlTime->data.timestamp;
			break;
		case SML_TIME_TIMESTAMP_LOCAL:
			secIndex = *smlTime->data.timestamp_local->timestamp;
			break;
		default:
			return NULL;
	}
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (18 ... 21)
	if (!list->status)
		return NULL;
	status = *(list->status->data.status32);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// OBIS word (22 ... 27)
	if (!list->obj_name || (list->obj_name->len != 6))
		return NULL;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (28)
	if (!list->unit)
		return NULL;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (29)
	if (!list->scaler)
		return NULL;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return NULL;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	signedContent = sml_octet_string_init(buf, sizeof(buf));

	return signedContent;
}

octet_string *sml_list_build_signed_content_bz_rlm(octet_string *server_id, sml_list *list)
{
	unsigned char buf[96] = {0};
	int pos = 0;
	octet_string *signedContent = NULL;
    sml_time *smlTime = NULL;
	u32 secIndex = 0;
    u32 status;
    u8 unit;
    i8 scaler;
    u64 value;

	if (!server_id || !list)
		return NULL;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56

	// device identification (0 ... 13)
	if (server_id->len >= 14)
		return NULL;

	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 14;

	// timestamp  (14 ... 17)
	smlTime = list->val_time;
	if (!smlTime)
		return NULL;
	switch (*(smlTime->tag))
	{
		case SML_TIME_SEC_INDEX:
			secIndex = *smlTime->data.sec_index;
			break;
		case SML_TIME_TIMESTAMP:
			secIndex = *smlTime->data.timestamp;
			break;
		case SML_TIME_TIMESTAMP_LOCAL:
			secIndex = *smlTime->data.timestamp_local->timestamp;
			break;
		default:
			return NULL;
	}
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);

	// status (18 ... 21)
	if (!list->status)
		return NULL;
	status = *(list->status->data.status32);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);

	// TODO: clarify where to find '+A' values and where to store the signature at the end
	// OBIS word '+A' (22 ... 27)
	if (!list->obj_name || (list->obj_name->len != 6))
		return NULL;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (28)
	if (!list->unit)
		return NULL;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (29)
	if (!list->scaler)
		return NULL;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return NULL;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	// TODO: clarify where to find 'R1' values and where to store the signature at the end
	// OBIS word 'R1' (38 ... 43)
	if (!list->obj_name || (list->obj_name->len != 6))
		return NULL;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (44)
	if (!list->unit)
		return NULL;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (45)
	if (!list->scaler)
		return NULL;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (46 ... 53)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return NULL;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	// TODO: clarify where to find 'R4' values and where to store the signature at the end
	// OBIS word 'R4' (54 ... 59)
	if (!list->obj_name || (list->obj_name->len != 6))
		return NULL;
	memcpy(buf + pos, list->obj_name->str, list->obj_name->len);
	pos += list->obj_name->len;

	// unit (60)
	if (!list->unit)
		return NULL;
	unit = *(list->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	// scale (61)
	if (!list->scaler)
		return NULL;
	scaler = *(list->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (62 ... 69)
	if (!list->value || (!(list->value->type & SML_TYPE_UNSIGNED) & !(list->value->type & SML_TYPE_INTEGER)))
		return NULL;
	value = *(list->value->data.uint64);
	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	signedContent = sml_octet_string_init(buf, sizeof(buf));

	return signedContent;
}

octet_string *sml_list_entry_build_signed_content(int type, octet_string *server_id, sml_list *list, octet_string *log_book)
{
	octet_string *signedContent = NULL;

	if (!server_id || !list)
		return NULL;

	if (type == SML_MESSAGE_TYPE_EDL)
	{
		// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38
		signedContent = sml_list_build_signed_content_edl(server_id, list, log_book);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_SLP) 
	{
		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
		signedContent = sml_list_build_signed_content_bz_slp(server_id, list);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_RLM) 
	{
		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
		signedContent = sml_list_build_signed_content_bz_rlm(server_id, list);
	}

	return signedContent;
}

octet_string *sml_tree_build_signed_content_bz_slp(octet_string *server_id, sml_tree *tree)
{
	unsigned char buf[96] = {0};
	int pos = 0;
	int found = 0;
	octet_string *signedContent = NULL;
	unsigned char parName[9] = { 0x01, 0x00, 0x01, 0x08, 0x00, 0xff, 0x80, 0x00, 0x00 };
    u32 secIndex = 0;
    int i = 0;
    u32 status = 0;
    sml_cosem_scaler_unit *smlCosemScalerUnit = NULL;
    u8 unit;
    i8 scaler;
	u64 value = 0;

	if (!server_id || !tree)
		return NULL;

	if (!tree->child_list || (tree->child_list_len < 4))
		return NULL;

	// 01 00 01 08 00 ff 80 00 00
	if (!tree->parameter_name || !tree->parameter_name->str)
		return NULL;
	if ((tree->parameter_name->len != sizeof(parName)) ||
		(memcmp(tree->parameter_name->str, parName, sizeof(parName)) != 0))
		return NULL;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56

	// device identification (0 ... 13)
	if (!server_id->str || server_id->len > 14)
		return NULL;
	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 14;

	// timestamp  (14 ... 17)
	for (i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x05 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							secIndex = *smlValue->data.uint32;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);
	found = 0;

	// status (18 ... 21)
	for (i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x04 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							status = *smlValue->data.uint32;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);
	found = 0;

	// OBIS word (22 ... 27)
	if (!tree->parameter_name->str)
		return NULL;
	memcpy(buf + pos, tree->parameter_name->str, 6);
	pos += 6;

	// unit (28) & scale (29)
	for (i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x03 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && (smlValue->type & SML_TYPE_LIST))
						{
							sml_list_type *smlListType = smlValue->data.smlList;
							if (smlListType && smlListType->tag && (*(smlListType->tag) == SML_LIST_TYPE_COSEM_VALUE))
							{
								sml_cosem_value *smlCosemValue = smlListType->data.cosem_value;
								if (smlCosemValue)
								{
									smlCosemScalerUnit = smlCosemValue->data.scaler_unit;
									found = 1;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (!smlCosemScalerUnit)
		return NULL;

	unit = *(smlCosemScalerUnit->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	scaler = *(smlCosemScalerUnit->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	for (i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x02 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							if (smlValue->type & 8)
								value = *smlValue->data.uint64;
							else if (smlValue->type & 4)
								value = *smlValue->data.uint32;
							else if (smlValue->type & 2)
								value = *smlValue->data.uint16;
							else if (smlValue->type & 1)
								value = *smlValue->data.uint8;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	signedContent = sml_octet_string_init(buf, sizeof(buf));

	return signedContent;
}

/*
octet_string *sml_tree_build_signed_content_bz_rlm(octet_string *server_id, sml_tree *tree)
{
	unsigned char buf[96] = {0};
	int pos = 0;
	int found = 0;
	octet_string *signedContent = NULL;

	if (!server_id || !tree)
		return NULL;

	if (!tree->child_list || (tree->child_list_len < 4))
		return NULL;

	// 01 00 01 08 00 ff 80 00 00
	if (!tree->parameter_name || !tree->parameter_name->str)
		return NULL;
	unsigned char parName[9] = { 0x01, 0x00, 0x01, 0x08, 0x00, 0xff, 0x80, 0x00, 0x00 };
	if ((tree->parameter_name->len != sizeof(parName)) ||
		(memcmp(tree->parameter_name->str, parName, sizeof(parName)) != 0))
		return NULL;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56

	// device identification (0 ... 13)
	if (!server_id->str || server_id->len > 14)
		return NULL;
	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 14;

	// timestamp  (14 ... 17)
	u32 secIndex = 0;
	for (int i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x05 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							secIndex = *smlValue->data.uint32;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);
	found = 0;

	// status (18 ... 21)
	u32 status = 0;
	for (int i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x04 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							status = *smlValue->data.uint32;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);
	found = 0;

	// OBIS word (22 ... 27)
	if (!tree->parameter_name->str)
		return NULL;
	memcpy(buf + pos, tree->parameter_name->str, 6);
	pos += 6;

	// unit (28) & scale (29)
	sml_cosem_scaler_unit *smlCosemScalerUnit = NULL;
	for (int i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x03 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && (smlValue->type & SML_TYPE_LIST))
						{
							sml_list_type *smlListType = smlValue->data.smlList;
							if (smlListType && smlListType->tag && (*(smlListType->tag) == SML_LIST_TYPE_COSEM_VALUE))
							{
								sml_cosem_value *smlCosemValue = smlListType->data.cosem_value;
								if (smlCosemValue)
								{
									smlCosemScalerUnit = smlCosemValue->data.scaler_unit;
									found = 1;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (!smlCosemScalerUnit)
		return NULL;

	u8 unit = *(smlCosemScalerUnit->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	i8 scaler = *(smlCosemScalerUnit->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (30 ... 37)
	u64 value = 0;
	for (int i = 0; i < tree->child_list_len; i++)
	{
		sml_tree *smlTree = tree->child_list[i];
		if (smlTree && smlTree->parameter_name)
		{
			unsigned char index[2] = { 0x00, 0x02 };
			if (smlTree->parameter_name->str && (smlTree->parameter_name->len == sizeof(index)))
			{
				if (memcmp(smlTree->parameter_name->str, index, sizeof(index)) == 0)
				{
					if (smlTree->parameter_value && smlTree->parameter_value->tag && (*(smlTree->parameter_value->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
					{
						sml_value *smlValue = smlTree->parameter_value->data.value;
						if (smlValue->type && ((smlValue->type & SML_TYPE_UNSIGNED) || (smlValue->type & SML_TYPE_INTEGER)))
						{
							if (smlValue->type & 8)
								value = *smlValue->data.uint64;
							else if (smlValue->type & 4)
								value = *smlValue->data.uint32;
							else if (smlValue->type & 2)
								value = *smlValue->data.uint16;
							else if (smlValue->type & 1)
								value = *smlValue->data.uint8;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return NULL;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN))
    {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	signedContent = sml_octet_string_init(buf, sizeof(buf));

	return signedContent;
}
*/

octet_string *sml_tree_build_signed_content(int type, octet_string *server_id, sml_tree *tree)
{
	octet_string *signedContent = NULL;

	if (!server_id || !tree)
		return NULL;

	if (type == SML_MESSAGE_TYPE_BZ_SLP) 
	{
		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
		signedContent = sml_tree_build_signed_content_bz_slp(server_id, tree);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_RLM) 
	{
		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
		//signedContent = sml_tree_build_signed_content_bz_rlm(server_id, tree);
	}

	return signedContent;
}
