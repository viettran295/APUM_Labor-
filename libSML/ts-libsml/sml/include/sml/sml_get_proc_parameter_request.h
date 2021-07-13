// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_PROC_PARAMETER_REQUEST_H_
#define _SML_GET_PROC_PARAMETER_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_tree.h"

//SML_GetProcParameter.Req ::= SEQUENCE
//{
//	serverId Octet String OPTIONAL,
//	username Octet String OPTIONAL,
//	password Octet String OPTIONAL,
//	parameterTreePath SML_TreePath
//	attribute Octet String OPTIONAL,
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    octet_string *server_id; // optional
    octet_string *username; // optional
    octet_string *password; // optional
    sml_tree_path *parameter_tree_path;
    octet_string *attribute; // optional
} sml_get_proc_parameter_request;

sml_get_proc_parameter_request *sml_get_proc_parameter_request_init(void);
sml_get_proc_parameter_request *sml_get_proc_parameter_request_parse(sml_buffer *buf);
int sml_get_proc_parameter_request_write(sml_get_proc_parameter_request *msg, sml_buffer *buf);
void sml_get_proc_parameter_request_free(sml_get_proc_parameter_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_PROC_PARAMETER_REQUEST_H_ */
