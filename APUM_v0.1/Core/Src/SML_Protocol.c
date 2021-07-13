#include "SML_Protocol.h"
#include <stdarg.h>
#include "TestP.h"
#include "Misc.h"
#include "LMN_ErrorMsg.h"

#define TRACE(_s, ...) do{ printf("[SMLP][%s]<%u> ", __FUNCTION__, __LINE__); printf(_s, ##__VA_ARGS__); printf("\r\n");} while(0)
#define ASSERT_B(_b, _r) do{if(!(_b)) {TRACE(LMN_GetErrorMessage(_r)); tempRet = _r; goto err;} } while(0)
#define ASSERT_F(_v) do{int32_t c = _v; if(c) {TRACE(LMN_GetErrorMessage(c)); tempRet = c; goto err;} } while(0)

//prototypes
sml_message *_SMLP_Message_GenerateGeneric(octet_string *transaction_id, sml_message_body *message_body, ...);
int32_t _SMLP_Message_Verify(sml_message *subject);

//IDs
uint8_t _SML_Client_ID[LEN_CLIENT_ID] = "SML Client";

//----------------------------------------------------------------
//region public functions

//----------------------------------------------------------------
//generate sml message for open response
sml_message *SMLP_Message_GenerateOpenRequest(uint8_t* server_id, uint16_t server_id_len, uint8_t* req_file_id, uint16_t req_file_id_len)
{	
	return _SMLP_Message_GenerateGeneric(
		NULL,
		NULL,
		SML_MESSAGE_OPEN_REQUEST, 
		NULL, 
		sml_octet_string_init(_SML_Client_ID, sizeof(_SML_Client_ID)),
		req_file_id ? sml_octet_string_init(req_file_id, req_file_id_len) : NULL,
		server_id ? sml_octet_string_init(server_id, server_id_len) : NULL,
		NULL,
		NULL,
		NULL);
}

//----------------------------------------------------------------
//generate sml message for close response
sml_message *SMLP_Message_GenerateCloseRequest()
{
	return _SMLP_Message_GenerateGeneric(
		NULL,
		NULL,
		SML_MESSAGE_CLOSE_REQUEST, 
		NULL);
}

//----------------------------------------------------------------
//generate sml message for get proc parameter request
sml_message *SMLP_Message_GenerateGetProcParameterRequest(uint8_t* server_id, uint16_t server_id_len, sml_tree_path* tree_path)
{		
	return _SMLP_Message_GenerateGeneric(
		NULL,
		NULL,
		SML_MESSAGE_GET_PROC_PARAMETER_REQUEST, 
		server_id ? sml_octet_string_init(server_id, server_id_len) : NULL,
		NULL,
		NULL,
		tree_path,
		NULL);
}

//----------------------------------------------------------------
//generate sml message for set proc parameter request
sml_message *SMLP_Message_GenerateSetProcParameterRequest(uint8_t* server_id, uint16_t server_id_len, sml_tree_path* tree_path, sml_tree* tree)
{		
	return _SMLP_Message_GenerateGeneric(
		NULL,
		NULL,
		SML_MESSAGE_SET_PROC_PARAMETER_REQUEST, 
		server_id ? sml_octet_string_init(server_id, server_id_len) : NULL,
		NULL,
		NULL,
		tree_path,
		tree);
}

//----------------------------------------------------------------
//verify sml file formally
//test for obligatory member except crc
//return SR_SML_ATTN_XXX to respond with attention otherwise default error behaviour applies
int32_t SMLP_File_Verify(sml_file *subject)
{
	int32_t tempRet = SML_E_Unknown;
	
	ASSERT_B((subject), SML_E_ParamInput);
	ASSERT_B((subject->messages && subject->messages_len >= 2), SML_E_FileMalformed);
	
	sml_message *tempFirst = subject->messages[0];
	sml_message *tempLast = subject->messages[subject->messages_len - 1];
	
	ASSERT_F(SMLP_Message_Verify(tempFirst));
	ASSERT_B(((*tempFirst->message_body->tag & 0xffffff00) == SML_MESSAGE_OPEN_REQUEST), SML_ATTN_UnexpectedSMLMessage);  //open req | resp
	
	//mem request | response
	uint8_t tempReqRes = *tempFirst->message_body->tag & 0xff;
	
	for (int i = 1; i < subject->messages_len - 1; i++)
	{
		ASSERT_F(SMLP_Message_Verify(subject->messages[i]));
		
		uint32_t tempTag = *subject->messages[i]->message_body->tag;
		ASSERT_B(((tempTag & 0xffffff00) != SML_MESSAGE_OPEN_REQUEST), SML_ATTN_UnexpectedSMLMessage);      //!open...
		ASSERT_B(((tempTag & 0xffffff00) != SML_MESSAGE_CLOSE_REQUEST), SML_ATTN_UnexpectedSMLMessage);      //!close...
		ASSERT_B(((tempTag & 0xff) == tempReqRes), SML_ATTN_UnexpectedSMLMessage);      //request | response...
	}
	
	ASSERT_F(SMLP_Message_Verify(tempLast));
	ASSERT_B(((*tempLast->message_body->tag & 0xffffff00) == SML_MESSAGE_CLOSE_REQUEST), SML_ATTN_UnexpectedSMLMessage);     //close req | resp
	ASSERT_B(((*tempLast->message_body->tag & 0xff) == tempReqRes), SML_ATTN_UnexpectedSMLMessage);         //request | response...
	
	return SML_Ok;
	
err:
	
	return tempRet;
}

//----------------------------------------------------------------
//verify sml message formally
//test for all obligatory member and consistency (for received and created messages) except crc
//sml_transport_write_file does not test for obligatory members!
//return SR_SML_ATTN_XXX to respond with attention otherwise default error behaviour applies
int32_t SMLP_Message_Verify(sml_message *subject)
{	
	int32_t tempRet = SML_E_Unknown;
	
	ASSERT_B(subject, SML_E_ParamInput);	
	
	ASSERT_B((TEST_P(subject, transaction_id, str) && subject->transaction_id->len > 0), SML_ATTN_RequestNotExecuted);
	ASSERT_B((subject->group_id && subject->abort_on_error && *subject->group_id == 0 && *subject->abort_on_error == 0), SML_ATTN_RequestNotExecuted);
	ASSERT_B((subject->message_body && subject->message_body->d.data && subject->message_body->tag), SML_ATTN_RequestNotExecuted);
	
	switch (*subject->message_body->tag)
	{
	case SML_MESSAGE_OPEN_REQUEST:
		{
			sml_open_request *tempSpec = subject->message_body->d.open_request;
			
			ASSERT_B((!tempSpec->codepage), SML_ATTN_UnsupportedOptElement);   //optional	
			ASSERT_B(((TEST_P(tempSpec, client_id, str)) && tempSpec->client_id->len == LEN_CLIENT_ID), SML_ATTN_Unknown);
			ASSERT_B(memcmp(tempSpec->client_id->str, _SML_Client_ID, LEN_CLIENT_ID) == 0, SML_ATTN_TargAddrNotAvailable);
			ASSERT_B(((TEST_P(tempSpec, req_file_id, str)) && tempSpec->req_file_id->len > 0), SML_ATTN_RequestNotAvailable);
			ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			ASSERT_B((!tempSpec->username), SML_ATTN_UnsupportedOptElement);   //optional
			ASSERT_B((!tempSpec->password), SML_ATTN_UnsupportedOptElement);   //optional
//			ASSERT_B((!tempSpec->sml_version), SML_ATTN_UnsupportedOptElement);  //optional		
				
			break;
		}
	case SML_MESSAGE_OPEN_RESPONSE:
		{
			sml_open_response *tempSpec = subject->message_body->d.open_response;
			
			ASSERT_B((!tempSpec->codepage), SML_ATTN_UnsupportedOptElement);   //optional	
			if(tempSpec->client_id)
			{
				ASSERT_B(TEST_P(tempSpec, client_id, str) && tempSpec->client_id->len == LEN_CLIENT_ID, SML_ATTN_Unknown);     //optional
				ASSERT_B(memcmp(tempSpec->client_id->str, _SML_Client_ID, LEN_CLIENT_ID) == 0, SML_ATTN_TargAddrNotAvailable);     //optional
			}
			ASSERT_B(((TEST_P(tempSpec, req_file_id, str)) && tempSpec->req_file_id->len > 0), SML_ATTN_RequestNotAvailable);
			ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			//			ASSERT_B((!tempSpec->ref_time), SML_ATTN_UnsupportedOptElement); //optional
			//			ASSERT_B((!tempSpec->sml_version), SML_ATTN_UnsupportedOptElement);  //optional	

						break;
		}
	case SML_MESSAGE_CLOSE_REQUEST:
		{
			sml_close_request *tempSpec = subject->message_body->d.close_request;
			
			ASSERT_B((!tempSpec->global_signature), SML_ATTN_UnsupportedOptElement);   //optional			
			
			break;
		}
	case SML_MESSAGE_CLOSE_RESPONSE:
		{
			sml_close_request *tempSpec = subject->message_body->d.close_response;
			
			ASSERT_B((!tempSpec->global_signature), SML_ATTN_UnsupportedOptElement);   //optional
			break;
		}
	case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
		{
			sml_get_proc_parameter_request *tempSpec = subject->message_body->d.get_proc_parameter_request;
			
			if (tempSpec->server_id)//optional
			{
				ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			}
			ASSERT_B((!tempSpec->username), SML_ATTN_UnsupportedOptElement);   //optional
			ASSERT_B((!tempSpec->password), SML_ATTN_UnsupportedOptElement);   //optional
			ASSERT_B(((TEST_P(tempSpec, parameter_tree_path, path_entries)) && tempSpec->parameter_tree_path->path_entries_len > 0), SML_ATTN_RequestNotExecuted);
			ASSERT_B((!tempSpec->attribute), SML_ATTN_UnsupportedOptElement);   //optional
			
			break;
		}
	case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
		{
			sml_get_proc_parameter_response *tempSpec = subject->message_body->d.get_proc_parameter_response;
			
			ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			ASSERT_B(((TEST_P(tempSpec, parameter_tree_path, path_entries)) && tempSpec->parameter_tree_path->path_entries_len > 0), SML_ATTN_RequestNotExecuted);
			ASSERT_B(((TEST_P(tempSpec, parameter_tree, child_list)) && tempSpec->parameter_tree->child_list_len > 0), SML_ATTN_RequestNotExecuted);
			
			break;
		}
	case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
		{
			sml_set_proc_parameter_request *tempSpec = subject->message_body->d.set_proc_parameter_request;

			if (tempSpec->server_id)//optional
			{
				ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			}
			ASSERT_B((!tempSpec->username), SML_ATTN_UnsupportedOptElement);   //optional
			ASSERT_B((!tempSpec->password), SML_ATTN_UnsupportedOptElement);   //optional
			ASSERT_B(((TEST_P(tempSpec, parameter_tree_path, path_entries)) && tempSpec->parameter_tree_path->path_entries_len > 0), SML_ATTN_RequestNotExecuted);
			ASSERT_B(((TEST_P(tempSpec, parameter_tree, child_list)) && tempSpec->parameter_tree->child_list_len > 0), SML_ATTN_RequestNotExecuted);
				
			break;
		}
	case SML_MESSAGE_ATTENTION_RESPONSE:
		{
			sml_attention_response *tempSpec = subject->message_body->d.attention_response;
			
			ASSERT_B(((TEST_P(tempSpec, server_id, str)) && tempSpec->server_id->len == LEN_SERVER_ID), SML_ATTN_TargAddrNotAvailable);
			ASSERT_B(((TEST_P(tempSpec, attention_number, str)) && tempSpec->attention_number->len == LEN_ATTN), SML_ATTN_Unknown);
			ASSERT_B((!tempSpec->attention_message || ((TEST_P(tempSpec, attention_message, str)) && tempSpec->attention_message->len > 0)), SML_ATTN_Unknown);   //optional
			ASSERT_B((!tempSpec->attention_details || ((TEST_P(tempSpec, attention_details, parameter_name, str)) && tempSpec->attention_details->parameter_name->len > 0)), SML_ATTN_Unknown);   //optional
			
			break;
		}
	default:
		{
			ASSERT_B(0, SML_ATTN_UnexpectedSMLMessage);
		}
	}
	return SML_Ok;
	
err:
	return tempRet;
}

//----------------------------------------------------------------
//region private functions

//----------------------------------------------------------------
//generate arbitrary sml message
//all parameter passed as NULL are not allocated! - except transaction_id and message body!
//if(message_body != NULL) variadic arguments are ignored
//else add int as SML_MESSAGE_TYPE + all members of specific body struct (can be NULL)
sml_message *_SMLP_Message_GenerateGeneric(octet_string *transaction_id, sml_message_body *message_body, ...)
{
	static uint32_t statTransID = 0;
	
#define _M_ON_ERR_() goto err
	
	va_list ap;
	va_start(ap, message_body);
	
	sml_message *tempRet = sml_message_init();
	if (!tempRet) _M_ON_ERR_();
	
	if (transaction_id)
	{
		if (tempRet->transaction_id) sml_octet_string_free(tempRet->transaction_id);
		tempRet->transaction_id = transaction_id;
	}
	else
#warning "redefine transaction ID"
	{
		if (tempRet->transaction_id) sml_octet_string_free(tempRet->transaction_id);
		uint32_t tempID = Misc_SwapByteOrder32(statTransID++);
		transaction_id = sml_octet_string_init((uint8_t*)&tempID, sizeof(tempID));
		tempRet->transaction_id = transaction_id;
	}
	
	tempRet->group_id = sml_u8_init(0);
	tempRet->abort_on_error = sml_u8_init(0);
	if (!tempRet->group_id || !tempRet->abort_on_error) _M_ON_ERR_();
	
	if (message_body) 
	{
		tempRet->message_body = message_body;
		return tempRet;
	}
	
	//no message body found
	
	int tempSMLMessageType = va_arg(ap, int);
	
	switch (tempSMLMessageType)
	{
	case SML_MESSAGE_OPEN_REQUEST:
		{
			sml_open_request *tempSpec = sml_open_request_init();
			if (!tempSpec) _M_ON_ERR_();
			
			tempSpec->codepage = va_arg(ap, octet_string*);
			tempSpec->client_id = va_arg(ap, octet_string*);
			tempSpec->req_file_id = va_arg(ap, octet_string*);
			tempSpec->server_id = va_arg(ap, octet_string*);
			tempSpec->username = va_arg(ap, octet_string*);
			tempSpec->password = va_arg(ap, octet_string*);
			tempSpec->sml_version = va_arg(ap, u8*);
			
			tempRet->message_body = sml_message_body_init2(tempSMLMessageType, tempSpec);
			if (!tempRet->message_body) _M_ON_ERR_();
			
			break;
		}
	case SML_MESSAGE_CLOSE_REQUEST:
		{
			sml_close_request *tempSpec = sml_close_request_init();
			if (!tempSpec) _M_ON_ERR_();
			
			tempSpec->global_signature = va_arg(ap, sml_signature*);
			
			tempRet->message_body = sml_message_body_init2(tempSMLMessageType, tempSpec);
			if (!tempRet->message_body) _M_ON_ERR_();
			
			break;
		}
	case SML_MESSAGE_GET_PROC_PARAMETER_REQUEST:
		{
			sml_get_proc_parameter_request *tempSpec = sml_get_proc_parameter_request_init();
			if (!tempSpec) _M_ON_ERR_();
			
			tempSpec->server_id = va_arg(ap, octet_string*);
			tempSpec->username = va_arg(ap, octet_string*);
			tempSpec->password = va_arg(ap, octet_string*);
			tempSpec->parameter_tree_path = va_arg(ap, sml_tree_path*);
			tempSpec->attribute = va_arg(ap, octet_string*);
			
			tempRet->message_body = sml_message_body_init2(tempSMLMessageType, tempSpec);
			if (!tempRet->message_body) _M_ON_ERR_();
			
			break;
		}
	case SML_MESSAGE_SET_PROC_PARAMETER_REQUEST:
		{
			sml_set_proc_parameter_request *tempSpec = sml_set_proc_parameter_request_init();
			if (!tempSpec) _M_ON_ERR_();
			
			tempSpec->server_id = va_arg(ap, octet_string*);
			tempSpec->username = va_arg(ap, octet_string*);
			tempSpec->password = va_arg(ap, octet_string*);
			tempSpec->parameter_tree_path = va_arg(ap, sml_tree_path*);
			tempSpec->parameter_tree = va_arg(ap, sml_tree*);
			
			tempRet->message_body = sml_message_body_init2(tempSMLMessageType, tempSpec);
			if (!tempRet->message_body) _M_ON_ERR_();
			
			break;
		}
	default:
		{
			_M_ON_ERR_();
		}
	}
	
	return tempRet;
	
err:
	sml_message_free(tempRet);
	return NULL;
	
#undef _M_ON_ERR_
}
