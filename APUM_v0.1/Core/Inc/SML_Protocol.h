#ifndef SML_PROTOCOL_H
#define SML_PROTOCOL_H

#include <stdio.h>
#include <stdint.h>
#include <sml/sml_file.h>

#define LEN_OBIS 6
#define LEN_OBIS_CLASS_VERS 9
#define LEN_OBIS_CLASS_VERS_METH 11
#define LEN_SERVER_ID 10
#define LEN_CLIENT_ID 10
#define LEN_ATTN 6

#define SML_ATTN_PREFIX 0x8181c7c70000

typedef enum
{
	SML_Ok = 0, 
	//SML_OK is already used in sml lib
	SML_E_Unknown = -3001,
	SML_E_ParamInput = -3002,
	SML_E_FileMalformed = -3003,
	
	//sml attention numbers section
	SML_ATTN_Ok = 0xfd00,
	SML_ATTN_RequestDelayed = 0xfd01,
	SML_ATTN_Unknown = 0xfe00,
	SML_ATTN_UnknownSMLIdentifier = 0xfe01,
	SML_ATTN_Unauthorized = 0xfe02,
	SML_ATTN_TargAddrNotAvailable = 0xfe03,
	SML_ATTN_RequestNotAvailable = 0xfe04,
	SML_ATTN_AttribWriteDenied = 0xfe05,
	SML_ATTN_AttribReadDenied = 0xfe06,
	SML_ATTN_MeterComError = 0xfe07,
	SML_ATTN_DataCorrupted = 0xfe08,
	SML_ATTN_ValueOutOfRange = 0xfe09,
	SML_ATTN_RequestNotExecuted = 0xfe0a,
	SML_ATTN_ChecksumError = 0xfe0b,
	SML_ATTN_BroadcastNotSupported = 0xfe0c,
	SML_ATTN_UnexpectedSMLMessage = 0xfe0d,
	SML_ATTN_UnknownObjectInProfile = 0xfe0e,
	SML_ATTN_UnsupportedDataType = 0xfe0f,
	SML_ATTN_UnsupportedOptElement = 0xfe10,
	SML_ATTN_ProfileEmpty = 0xfe11,
	SML_ATTN_ProfileRangeError = 0xfe12,
	SML_ATTN_ProfileRangeEmpty = 0xfe13,
	SML_ATTN_CloseMissing = 0xfe14,
	SML_ATTN_ProfileTempUnavailable = 0xfe15,
}SML_RetVal_t;

int32_t SMLP_File_Verify(sml_file *subject);
int32_t SMLP_Message_Verify(sml_message *subject);

sml_message *SMLP_Message_GenerateOpenRequest(uint8_t* server_id, uint16_t server_id_len, uint8_t* req_file_id, uint16_t req_file_id_len);
sml_message *SMLP_Message_GenerateCloseRequest();
sml_message *SMLP_Message_GenerateGetProcParameterRequest(uint8_t* server_id, uint16_t server_id_len, sml_tree_path* tree_path);
sml_message *SMLP_Message_GenerateSetProcParameterRequest(uint8_t* server_id, uint16_t server_id_len, sml_tree_path* tree_path, sml_tree* tree);

#endif //SML_PROTOCOL_H