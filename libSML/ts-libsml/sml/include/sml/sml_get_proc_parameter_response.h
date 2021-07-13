// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_PROC_PARAMETER_RESPONSE_H_
#define _SML_GET_PROC_PARAMETER_RESPONSE_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_tree.h"

//SML_GetProcParameter.Res ::= SEQUENCE
//{
//	serverId Octet String,
//	parameterTreePath SML_TreePath,
//	parameterTree SML_Tree
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *server_id;
	sml_tree_path *parameter_tree_path;
	sml_tree *parameter_tree;
} sml_get_proc_parameter_response;

sml_get_proc_parameter_response *sml_get_proc_parameter_response_init(void);
sml_get_proc_parameter_response *sml_get_proc_parameter_response_parse(sml_buffer *buf);
int sml_get_proc_parameter_response_write(sml_get_proc_parameter_response *msg, sml_buffer *buf);
void sml_get_proc_parameter_response_free(sml_get_proc_parameter_response *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_PROC_PARAMETER_RESPONSE_H_ */
