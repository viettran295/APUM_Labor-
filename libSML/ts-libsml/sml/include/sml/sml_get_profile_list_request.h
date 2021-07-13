// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_PROFILE_LIST_REQUEST_H_
#define _SML_GET_PROFILE_LIST_REQUEST_H_

#include "sml_get_profile_pack_request.h"

//SML_GetProfileList.Req ::= SEQUENCE
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

#ifdef __cplusplus
extern "C" {
#endif

// Apparently SML_GetProfilePack.Req is the same as SML_GetProfileList.Req
typedef sml_get_profile_pack_request sml_get_profile_list_request;

#define sml_get_profile_list_request_init() sml_get_profile_pack_request_init()
#define sml_get_profile_list_request_parse(buf) sml_get_profile_pack_request_parse(buf)
#define sml_get_profile_list_request_write(msg, buf) sml_get_profile_pack_request_write(msg, buf)
#define sml_get_profile_list_request_free(msg) sml_get_profile_pack_request_free(msg)

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_PROFILE_LIST_REQUEST_H_ */
