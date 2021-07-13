// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_GET_LIST_REQUEST_H_
#define _SML_GET_LIST_REQUEST_H_

#include "sml_shared.h"
#include "sml_octet_string.h"
#include "sml_time.h"
#include "sml_list.h"

//SML_GetList.Req ::= SEQUENCE
//{
//	clientId Octet String,
//	serverId Octet String OPTIONAL,
//	username Octet String OPTIONAL,
//	password Octet String OPTIONAL,
//	listName Octet String OPTIONAL
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	octet_string *client_id;
	octet_string *server_id;    // optional
	octet_string *username;		// optional
	octet_string *password;		// optional
	octet_string *list_name; 	// optional
} sml_get_list_request;

sml_get_list_request* sml_get_list_request_init(void);
sml_get_list_request *sml_get_list_request_parse(sml_buffer *buf);
int sml_get_list_request_write(sml_get_list_request *msg, sml_buffer *buf);
void sml_get_list_request_free(sml_get_list_request *msg);

#ifdef __cplusplus
}
#endif

#endif /* _SML_GET_LIST_REQUEST_H_ */
