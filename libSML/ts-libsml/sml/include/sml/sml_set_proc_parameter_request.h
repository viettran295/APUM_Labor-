// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_SET_PROC_PARAMETER_REQUEST_H_
#define _SML_SET_PROC_PARAMETER_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_tree.h"

//SML_SetProcParameter.Req ::= SEQUENCE
//{
//	serverId Octet String OPTIONAL,
//	username Octet String OPTIONAL,
//	password Octet String OPTIONAL,
//	parameterTreePath SML_TreePath,
//	parameterTree SML_Tree
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *server_id; // optional
	octet_string *username; // optional
	octet_string *password; // optional
	sml_tree_path *parameter_tree_path;
	sml_tree *parameter_tree;
} sml_set_proc_parameter_request;

sml_set_proc_parameter_request *sml_set_proc_parameter_request_init(void);
sml_set_proc_parameter_request *sml_set_proc_parameter_request_parse(sml_buffer *buf);
int sml_set_proc_parameter_request_write(sml_set_proc_parameter_request *msg, sml_buffer *buf);
void sml_set_proc_parameter_request_free(sml_set_proc_parameter_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_SET_PROC_PARAMETER_REQUEST_H_ */

