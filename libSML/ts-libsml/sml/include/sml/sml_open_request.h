// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_OPEN_REQUEST_H_
#define	_SML_OPEN_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"

//SML_PublicOpen.Req ::= SEQUENCE
//{
//	codepage Octet String OPTIONAL,
//	clientId Octet String
//	reqFileId Octet String
//	serverId Octet String OPTIONAL,
//	username Octet String OPTIONAL,
//	password Octet String OPTIONAL,
//	smlVersion Unsigned8 OPTIONAL,
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    octet_string *codepage; // optional
	octet_string *client_id;
	octet_string *req_file_id;
	octet_string *server_id; // optional
	octet_string *username; // optional
	octet_string *password; // optional
	u8 *sml_version; // optional
} sml_open_request;

sml_open_request *sml_open_request_init(void);
sml_open_request *sml_open_request_parse(sml_buffer *buf);
int sml_open_request_write(sml_open_request *msg, sml_buffer *buf);
void sml_open_request_free(sml_open_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_OPEN_REQUEST_H_ */
