// Copyright 2015 Uwe Heuert


#include <stdio.h>
#include <sml/sml_signature.h>
#include <sml/sml_message.h>

int sml_tree_build_signed_content_bz_edl(octet_string *server_id, sml_tree *tree, unsigned char *buf, size_t len)
{
	int pos = 0;
	int found = 0;

	if (!tree || !server_id || !buf || (len != 48))
		return 0;

	if (!tree->child_list || (tree->child_list_len < 4))
		return 0;

	// 01 00 01 08 00 ff 80 00 00
	if (!tree->parameter_name || !tree->parameter_name->str)
		return 0;
	unsigned char parName[9] = { 0x01, 0x00, 0x01, 0x08, 0x00, 0xff, 0x80, 0x00, 0x00 };
	if ((tree->parameter_name->len != sizeof(parName)) ||
		(memcmp(tree->parameter_name->str, parName, sizeof(parName)) != 0))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38
	memset(buf + pos, 0, len);

	// device identification (0 ... 9)
	if (!server_id->str || server_id->len > 10)
		return 0;
	memcpy(buf + pos, server_id->str, server_id->len);
	pos += 10;

	// timestamp  (10 ... 13)
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
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&secIndex, sizeof(secIndex));
	}
	memcpy(buf + pos, &secIndex, sizeof(secIndex));
	pos += sizeof(secIndex);
	found = 0;

	// status (14)
	u8 status = 0;
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
							status = *smlValue->data.uint8;
							found = 1;
							break;
						}
					}
				}
			}
		}
	}
	if (!found)
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);
	found = 0;

	// OBIS word (15 ... 20)
	if (!tree->parameter_name->str)
		return 0;
	memcpy(buf + pos, tree->parameter_name->str, 6);
	pos += 6;

	// unit (21) & scale (22)
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
		return 0;

	u8 unit = *(smlCosemScalerUnit->unit);
	memcpy(buf + pos, &unit, sizeof(unit));
	pos += sizeof(unit);

	i8 scaler = *(smlCosemScalerUnit->scaler);
	memcpy(buf + pos, &scaler, sizeof(scaler));
	pos += sizeof(scaler);

	// meter count (23 ... 30)
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
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	return 1;
}

int sml_tree_build_signed_content_bz_slp(octet_string *server_id, sml_tree *tree, unsigned char *buf, size_t len)
{
	int pos = 0;
	int found = 0;

	if (!tree || !server_id || !buf || (len != 96))
		return 0;

	if (!tree->child_list || (tree->child_list_len < 4))
		return 0;

	// 01 00 01 08 00 ff 80 00 00
	if (!tree->parameter_name || !tree->parameter_name->str)
		return 0;
	unsigned char parName[9] = { 0x01, 0x00, 0x01, 0x08, 0x00, 0xff, 0x80, 0x00, 0x00 };
	if ((tree->parameter_name->len != sizeof(parName)) ||
		(memcmp(tree->parameter_name->str, parName, sizeof(parName)) != 0))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
	memset(buf + pos, 0, len);

	// device identification (0 ... 13)
	if (!server_id->str || server_id->len > 14)
		return 0;
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
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
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
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&status, sizeof(status));
	}
	memcpy(buf + pos, &status, sizeof(status));
	pos += sizeof(status);
	found = 0;

	// OBIS word (22 ... 27)
	if (!tree->parameter_name->str)
		return 0;
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
		return 0;

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
		return 0;

	if (!(sml_number_endian() == SML_LITTLE_ENDIAN)) {
		sml_number_byte_swap((unsigned char*)&value, sizeof(value));
	}
	memcpy(buf + pos, &value, sizeof(value));
	pos += sizeof(value);

	return 1;
}
/*
int sml_tree_build_signed_content_bz_rlm(sml_tree *tree, octet_string *server_id, unsigned char *buf, size_t len)
{
	int pos = 0;

	if (!tree || !server_id || !buf || (len != 96))
		return 0;

	// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
	memset(buf + pos, 0, len);

	// device identification (0 ... 13)
	if (!server_id->str || server_id->len > 14)
		return 0;
	memcpy(buf + pos, server_id->str, server_id->len);
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
*/
void sml_get_proc_parameter_response_build_signed_content(int type, sml_get_proc_parameter_response *smlGetProcParameterResponse, unsigned char *buf, size_t len)
{
	if (!smlGetProcParameterResponse || !smlGetProcParameterResponse->server_id || !smlGetProcParameterResponse->parameter_tree || !smlGetProcParameterResponse->parameter_tree->child_list || (!buf))
		return;

	if (type == SML_MESSAGE_TYPE_EDL)
	{
		if ((smlGetProcParameterResponse->server_id->len > 10) || (len != 48))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft EDL starting on page 38
		//sml_tree_build_signed_content_edl(smlGetProcParameterResponse->server_id, smlGetProcParameterResponse->parameter_tree, buf, len);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_SLP) 
	{
		if ((smlGetProcParameterResponse->server_id->len > 14) || (len != 96))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 56
		sml_tree_build_signed_content_bz_slp(smlGetProcParameterResponse->server_id, smlGetProcParameterResponse->parameter_tree, buf, len);
	}
	else if (type == SML_MESSAGE_TYPE_BZ_RLM) 
	{
		if ((smlGetProcParameterResponse->server_id->len > 14) || (len != 96))
			return;

		// calculation of hash code (input for hash algo) according Lastenheft BZ starting on page 75
		//sml_tree_build_signed_content_bz_rlm(smlGetProcParameterResponse->server_id, smlGetProcParameterResponse->parameter_tree, buf, len);
	}
	else
	{
		return;
	}

	return;
}
