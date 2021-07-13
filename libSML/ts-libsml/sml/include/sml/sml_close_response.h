// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_CLOSE_RESPONSE_H_
#define _SML_CLOSE_RESPONSE_H_

#include "sml_close_request.h"

//SML_PublicClose.Res ::= SEQUENCE
//{
//	globalSignature SML_Signature OPTIONAL
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef sml_close_request sml_close_response;

sml_close_response *sml_close_response_init(void);
sml_close_response *sml_close_response_parse(sml_buffer *buf);
int sml_close_response_write(sml_close_response *msg, sml_buffer *buf);
void sml_close_response_free(sml_close_response *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_CLOSE_RESPONSE_H_ */
