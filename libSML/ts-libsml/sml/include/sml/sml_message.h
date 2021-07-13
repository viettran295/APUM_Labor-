// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_MESSAGE_H_
#define	_SML_MESSAGE_H_

#include "sml_octet_string.h"
#include "sml_time.h"
#include "sml_list.h"
#include "sml_tree.h"

#include "sml_open_request.h"
#include "sml_open_response.h"
#include "sml_close_request.h"
#include "sml_close_response.h"
#include "sml_get_profile_pack_request.h"
#include "sml_get_profile_pack_response.h"
#include "sml_get_profile_list_request.h"
#include "sml_get_profile_list_response.h"
#include "sml_get_proc_parameter_request.h"
#include "sml_get_proc_parameter_response.h"
#include "sml_set_proc_parameter_request.h"
#include "sml_get_list_request.h"
#include "sml_get_list_response.h"
#include "sml_attention_response.h"

//SML_Message ::= SEQUENCE
//{
//	transactionId Octet String,
//	groupNo Unsigned8,
//	abortOnError Unsigned8,
//	messageBody SML_MessageBody,
//	crc16 Unsigned16,
//	endOfSmlMsg EndOfSmlMsg
//}

//SML_MessageBody ::= CHOICE
//{
//	OpenRequest [0x00000100] SML_PublicOpen.Req
//	OpenResponse [0x00000101] SML_PublicOpen.Res
//	CloseRequest [0x00000200] SML_PublicClose.Req
//	CloseResponse [0x00000201] SML_PublicClose.Res
//	GetProfilePackRequest [0x00000300] SML_GetProfilePack.Req
//	GetProfilePackResponse [0x00000301] SML_GetProfilePack.Res
//	GetProfileListRequest [0x00000400] SML_GetProfileList.Req
//	GetProfileListResponse [0x00000401] SML_GetProfileList.Res
//	GetProcParameterRequest [0x00000500] SML_GetProcParameter.Req
//	GetProcParameterResponse [0x00000501] SML_GetProcParameter.Res
//	SetProcParameterRequest [0x00000600] SML_SetProcParameter.Req
//	GetListRequest [0x00000700] SML_GetList.Req
//	GetListResponse [0x00000701] SML_GetList.Res
//	AttentionResponse [0x0000FF01] SML_Attention.Res
//}

#define SML_MESSAGE_OPEN_REQUEST			0x00000100
#define SML_MESSAGE_OPEN_RESPONSE			0x00000101
#define SML_MESSAGE_CLOSE_REQUEST			0x00000200
#define SML_MESSAGE_CLOSE_RESPONSE			0x00000201
#define SML_MESSAGE_GET_PROFILE_PACK_REQUEST		0x00000300
#define SML_MESSAGE_GET_PROFILE_PACK_RESPONSE		0x00000301
#define SML_MESSAGE_GET_PROFILE_LIST_REQUEST		0x00000400
#define SML_MESSAGE_GET_PROFILE_LIST_RESPONSE		0x00000401
#define SML_MESSAGE_GET_PROC_PARAMETER_REQUEST		0x00000500
#define SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE		0x00000501
#define SML_MESSAGE_SET_PROC_PARAMETER_REQUEST		0x00000600
#define SML_MESSAGE_SET_PROC_PARAMETER_RESPONSE		0x00000601 // This doesn't exist in the spec
#define SML_MESSAGE_GET_LIST_REQUEST			0x00000700
#define SML_MESSAGE_GET_LIST_RESPONSE			0x00000701
#define SML_MESSAGE_ATTENTION_RESPONSE			0x0000FF01

#define SML_MESSAGE_TYPE_SYM2				1
#define SML_MESSAGE_TYPE_EDL				2
#define SML_MESSAGE_TYPE_BZ_SLP				3
#define SML_MESSAGE_TYPE_BZ_RLM				4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	u32 *tag;
	//void *data;
	union {
		void *data;
		sml_open_request *open_request;
		sml_open_response *open_response;
		sml_close_request *close_request;
		sml_close_response *close_response;
		sml_get_profile_pack_request *get_profile_pack_request;
		sml_get_profile_pack_response *get_profile_pack_response;
		sml_get_profile_list_request *get_profile_list_request;
		sml_get_profile_list_response *get_profile_list_response;
		sml_get_proc_parameter_request *get_proc_parameter_request;
		sml_get_proc_parameter_response *get_proc_parameter_response;
		sml_set_proc_parameter_request *set_proc_parameter_request;
		sml_get_list_request *get_list_request;
		sml_get_list_response *get_list_response;
		sml_attention_response *attention_response;
	} d;
} sml_message_body;

typedef struct {
	octet_string *transaction_id;
	u8 *group_id;
	u8 *abort_on_error;
	sml_message_body *message_body;
	u16 *crc;
	/* end of message */
} sml_message;

// SML MESSAGE
sml_message *sml_message_init(void); // Sets a transaction id.
sml_message *sml_message_parse(sml_buffer *buf);
int sml_message_write(sml_message *msg, sml_buffer *buf);
void sml_message_free(sml_message *msg);

// SML_MESSAGE_BODY
sml_message_body *sml_message_body_init();
sml_message_body *sml_message_body_init2(u32 tag, void *data);
sml_message_body *sml_message_body_parse(sml_buffer *buf);
int sml_message_body_write(sml_message_body *message_body, sml_buffer *buf);
void sml_message_body_free(sml_message_body *message_body);

#ifdef __cplusplus
}
#endif

#endif /* _SML_MESSAGE_H_ */
