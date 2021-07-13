#include "LMN_ErrorMsg.h"
//#include <HDLC_Factory.h>
//#include "HDLC_HL_Client.h"
//#include "HDLC_Meter.h"
//#include "Serial.h"
#include "SML_Factory.h"

typedef struct 
{
	int32_t Value;
	char* String;
}
ErrorMsgEntry_t;

static ErrorMsgEntry_t Messages[] = 
{ 
	{ 0,								"[any] no error"},
	
//	{ SMLF_OK,							"[SMLF] ok"},
	{ SMLF_E_Unknown,					"[SMLF] unknown error"},
	{ SMLF_E_ParamInput,				"[SMLF] input parameter error"},
	{ SMLF_E_Malloc,					"[SMLF] out of memory error"},
	{ SMLF_E_COSEMClassType,			"[SMLF] COSEM class type error"},
	{ SMLF_E_COSEMClassProperties,		"[SMLF] COSEM class properties error"},
	{ SMLF_E_SMLFileStructure,			"[SMLF] sml file structure error"},
	{ SMLF_E_SMLMessageType,			"[SMLF] sml message type error"},
	{ SMLF_E_SMLAttention,				"[SMLF] sml attention error"},
	{ SMLF_E_SMLGPPRespMalformed,		"[SMLF] sml GetProcParameterResponse message malformed error"},
	{ SMLF_E_SMLValueMalformed,			"[SMLF] sml value malformed error"},
	{ SMLF_E_OBISUnknown,				"[SMLF] OBIS code unknown error"},
	{ SMLF_E_ValueTypeMismatch,			"[SMLF] value type mismatch error"},
};

//----------------------------------------------------------------
//get error message from enum value
char* LMN_GetErrorMessage(int32_t inValue)
{
	for (uint16_t i = 0; i < sizeof(Messages) / sizeof(ErrorMsgEntry_t); i++)
		if (Messages[i].Value == inValue) return Messages[i].String;

	return "unknown error value";
}
