// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_PROFILE_PACK_REQUEST_H_
#define _SML_GET_PROFILE_PACK_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_time.h"
#include "sml_list.h"
#include "sml_tree.h"

//SML_GetProfilePack.Req ::= SEQUENCE
//{
//	serverId Octet String OPTIONAL,
//	username Octet String OPTIONAL,
//	password Octet String OPTIONAL,
//	withRawdata Boolean OPTIONAL,
//	beginTime SML_Time OPTIONAL,
//	endTime SML_Time OPTIONAL,
//	parameterTreePath SML_TreePath,
//	object_List List_of_SML_ObjReqEntry OPTIONAL,
//	dasDetails SML_Tree OPTIONAL
//}

//List_of_SML_ObjReqEntry ::= SEQUENCE OF
//{
//	object_List_Entry SML_ObjReqEntry
//}

//SML_ObjReqEntry ::= Octet String

#ifdef __cplusplus
extern "C" {
#endif

typedef octet_string sml_obj_req_entry;
#define sml_obj_req_entry_parse(buf) sml_octet_string_parse(buf)
#define sml_obj_req_entry_write(p, buf) sml_octet_string_write(p, buf)
#define sml_obj_req_entry_free(p) sml_octet_string_free(p)

typedef struct sml_obj_req_entry_list_entry {
	sml_obj_req_entry *object_list_entry;

	// list specific
	struct sml_obj_req_entry_list_entry *next;
} sml_obj_req_entry_list;

typedef struct {
	octet_string *server_id;	// optional
	octet_string *username; 	//  optional
	octet_string *password; 	//  optional
	sml_boolean *with_rawdata;  // optional
	sml_time *begin_time;		// optional
	sml_time *end_time;			// optional
	sml_tree_path *parameter_tree_path;
	sml_obj_req_entry_list *object_list; // optional
	sml_tree *das_details;		// optional
} sml_get_profile_pack_request;

sml_get_profile_pack_request *sml_get_profile_pack_request_init(void);
sml_get_profile_pack_request *sml_get_profile_pack_request_parse(sml_buffer *buf);
int sml_get_profile_pack_request_write(sml_get_profile_pack_request *msg, sml_buffer *buf);
void sml_get_profile_pack_request_free(sml_get_profile_pack_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_PROFILE_PACK_REQUEST_H_ */
