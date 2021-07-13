// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_PROFILE_PACK_RESPONSE_H_
#define _SML_GET_PROFILE_PACK_RESPONSE_H_

//SML_GetProfilePack.Res ::= SEQUENCE
//{
//	serverId Octet String,
//	actTime SML_Time,
//	regPeriod Unsigned32,
//	parameterTreePath SML_TreePath,
//	header_List List_of_SML_ProfObjHeaderEntry,
//	period_List List_of_SML_ProfObjPeriodEntry,
//	rawdata Octet String OPTIONAL,
//	profileSignature SML_Signature OPTIONAL
//}

//List_of_SML_ProfObjHeaderEntry ::= SEQUENCE OF
//{
//	header_List_Entry SML_ProfObjHeaderEntry
//}

//SML_ProfObjHeaderEntry ::= SEQUENCE
//{
//	objName Octet String,
//	unit SML_Unit,
//	scaler Integer8
//}

//List_of_SML_ProfObjPeriodEntry ::= SEQUENCE OF
//{
//	period_List_Entry SML_ProfObjPeriodEntry
//}

//SML_ProfObjPeriodEntry ::= SEQUENCE
//{
//	valTime SML_Time,
//	status Unsigned64,
//	value_List List_of_SML_ValueEntry,
//	periodSignature SML_Signature OPTIONAL
//}

//List_of_SML_ValueEntry ::= SEQUENCE OF
//{
//	value_List_Entry SML_ValueEntry
//}

//SML_ValueEntry ::= SEQUENCE
//{
//	value SML_Value,
//	valueSignature SML_Signature OPTIONAL
//}

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_time.h"
#include "sml_list.h"
#include "sml_tree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *server_id;
	sml_time *act_time; // specified by germans (current time was meant) ;)
	u32 *reg_period;
	sml_tree_path *parameter_tree_path;
	sml_sequence *header_list; 			// list of sml_prof_obj_header_entry
	sml_sequence *period_list;			// list of sml_prof_obj_period_entry
	octet_string *rawdata;  			// optional
	sml_signature *profile_signature; 	// optional

} sml_get_profile_pack_response;

sml_get_profile_pack_response *sml_get_profile_pack_response_init(void);
sml_get_profile_pack_response *sml_get_profile_pack_response_parse(sml_buffer *buf);
int sml_get_profile_pack_response_write(sml_get_profile_pack_response *msg, sml_buffer *buf);
void sml_get_profile_pack_response_free(sml_get_profile_pack_response *msg);

typedef struct {
	octet_string *obj_name;
	sml_unit *unit;
	i8 *scaler;
} sml_prof_obj_header_entry;

sml_prof_obj_header_entry *sml_prof_obj_header_entry_init(void);
sml_prof_obj_header_entry *sml_prof_obj_header_entry_parse(sml_buffer *buf);
int sml_prof_obj_header_entry_write(sml_prof_obj_header_entry *entry, sml_buffer *buf);
void sml_prof_obj_header_entry_free(sml_prof_obj_header_entry *entry);

typedef struct {
	sml_time *val_time;
	u64 *status;
	sml_sequence *value_list;
	sml_signature *period_signature;
} sml_prof_obj_period_entry;

sml_prof_obj_period_entry *sml_prof_obj_period_entry_init(void);
sml_prof_obj_period_entry *sml_prof_obj_period_entry_parse(sml_buffer *buf);
int sml_prof_obj_period_entry_write(sml_prof_obj_period_entry *entry, sml_buffer *buf);
void sml_prof_obj_period_entry_free(sml_prof_obj_period_entry *entry);

typedef struct {
	sml_value *value;
	sml_signature *value_signature;
} sml_value_entry;

sml_value_entry *sml_value_entry_init(void);
sml_value_entry *sml_value_entry_parse(sml_buffer *buf);
int sml_value_entry_write(sml_value_entry *entry, sml_buffer *buf);
void sml_value_entry_free(sml_value_entry *entry);

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_PROFILE_PACK_RESPONSE_H_ */
