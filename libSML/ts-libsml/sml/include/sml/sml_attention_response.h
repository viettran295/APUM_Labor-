// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_ATTENTION_RESPONSE_H_
#define	_SML_ATTENTION_RESPONSE_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_tree.h"

//SML_Attention.Res ::= SEQUENCE
//{
//	serverId Octet String,
//	attentionNo Octet String,
//	attentionMsg Octet String OPTIONAL,
//	attentionDetails SML_Tree OPTIONAL
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *server_id;
	octet_string *attention_number;
	octet_string *attention_message; // optional
	sml_tree *attention_details;	 // optional
} sml_attention_response;

sml_attention_response *sml_attention_response_init(void);
sml_attention_response *sml_attention_response_parse(sml_buffer *buf);
int sml_attention_response_write(sml_attention_response *msg, sml_buffer *buf);
void sml_attention_response_free(sml_attention_response *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_ATTENTION_RESPONSE_H_ */
