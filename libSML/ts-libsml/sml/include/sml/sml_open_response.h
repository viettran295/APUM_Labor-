// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_OPEN_RESPONSE_H_
#define	_SML_OPEN_RESPONSE_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_time.h"

//SML_PublicOpen.Res ::= SEQUENCE
//{
//	codepage Octet String OPTIONAL,
//	clientId Octet String OPTIONAL,
//	reqFileId Octet String,
//	serverId Octet String,
//	refTime SML_Time OPTIONAL,
//	smlVersion Unsigned8 OPTIONAL,
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *codepage; // optional
	octet_string *client_id; // optional
	octet_string *req_file_id;
	octet_string *server_id;
	sml_time *ref_time; // optional
	u8 *sml_version; // optional
} sml_open_response;

sml_open_response *sml_open_response_init(void);
sml_open_response *sml_open_response_parse(sml_buffer *buf);
int sml_open_response_write(sml_open_response *msg, sml_buffer *buf);
void sml_open_response_free(sml_open_response *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_OPEN_RESPONSE_H_ */
