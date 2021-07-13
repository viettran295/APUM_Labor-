// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_CLOSE_REQUEST_H_
#define _SML_CLOSE_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"

//SML_PublicClose.Req ::= SEQUENCE
//{
//	globalSignature SML_Signature OPTIONAL
//}

//SML_Signature ::= Octet String

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	sml_signature *global_signature; // optional
} sml_close_request;

sml_close_request *sml_close_request_init(void);
sml_close_request * sml_close_request_parse(sml_buffer *buf);
int sml_close_request_write(sml_close_request *msg, sml_buffer *buf);
void sml_close_request_free(sml_close_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_CLOSE_REQUEST_H_ */
