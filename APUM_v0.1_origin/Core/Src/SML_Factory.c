#include "SML_Factory.h"
#include "SML_Protocol.h"
#include "LMN_ErrorMsg.h"
//#include <arpa/inet.h>
#include "Misc.h"
#include "sml/sml_transport.h"
#include "TestP.h"
#include "OBISMapping.h"
#include <FreeRTOS.h>

#define malloc pvPortMalloc
#define free vPortFree
#define realloc pvPortRealloc

#define TRACE(_s, ...) do{ printf("[SMLF][%s]<%u> ", __FUNCTION__, __LINE__); printf(_s, ##__VA_ARGS__); printf("\r\n");} while(0)
#define ASSERT_B(_b, _r) do{if(!(_b)) {TRACE(LMN_GetErrorMessage(_r)); tempRet = _r; goto err;} } while(0)
#define ASSERT_F(_v) do{int32_t c = _v; if(c) {TRACE(LMN_GetErrorMessage(c)); tempRet = c; goto err;} } while(0)

uint32_t _ReqFileID = 0;

//prototypes
int32_t _SMLF_CreateGetPPReqMessage(OBISMapping_t* inOM, sml_message** outM);
int32_t _SMLF_ParseGetPPRespMessage(sml_get_proc_parameter_response* inMsg, OBISDataChain_t** outData);
int32_t _SMLF_CreateSetPPReqMessage(OBISMapping_t* inOM, uCOSEMValue_t* inValue, sml_message** outM);
int32_t _SMLF_ParseCOSEMClass_1(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData);
int32_t _SMLF_ParseCOSEMClass_3(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData);
int32_t _SMLF_ParseCOSEMClass_32770(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData);
int32_t _SMLF_TxferSMLValue2OBISData(OBISMapping_t* inOM, sml_value* inSMLValue, uCOSEMValue_t* inCValue);

uint8_t _SML_Server_ID[LEN_SERVER_ID] = "SML Server";

//--------------------------------
//sml file handling

//----------------------------------------------------------------
//free file ressources recursively
void SMLF_FileFree(sml_file* inFile)
{
	sml_file_free(inFile);
}

//----------------------------------------------------------------
//get buffer of file
uint8_t* SMLF_FileGetBuf(sml_file* inFile)
{
	if (!(TEST_P(inFile, buf, buffer))) return NULL;
	return inFile->buf->buffer;
}

//----------------------------------------------------------------
//get buffer len of file
uint16_t SMLF_FileGetBufLen(sml_file* inFile)
{
	return sml_file_get_buffer_len(inFile);
}

//----------------------------------------------------------------
//parse sml file from buffer
sml_file* SMLF_FileParseFromBuf(uint8_t* inData, uint16_t inLen)
{
	uint8_t tempBuf[inLen];
	inLen = sml_transport_read(inData, tempBuf, inLen);	
	return sml_file_parse(tempBuf, inLen, 0);
}

//--------------------------------
//sml file creation

//----------------------------------------------------------------
//create sml file containing GPPReq messages for OBIS parameter
int32_t SMLF_CreateGetPPReqFilePID(ParamID_t inPID, sml_file** outFile)
{
	return SMLF_CreateGetPPReqFilePID2(&inPID, 1, outFile);
}

//----------------------------------------------------------------
//create sml file containing GPPReq messages for array of OBIS parameter
int32_t SMLF_CreateGetPPReqFilePID2(ParamID_t* inPIDArray, uint16_t inLen, sml_file** outFile)
{
	int32_t tempRet = SMLF_E_Unknown;
	OBISMapping_t* tempOM[inLen];
	
	ASSERT_B(inPIDArray && inLen > 0, SMLF_E_ParamInput);
	
	memset(tempOM, 0, inLen* sizeof(OBISMapping_t*));
	
	for (size_t i = 0; i < inLen; i++)
		tempOM[i] = OBIS_MappingGet(inPIDArray[i]);
	
	return SMLF_CreateGetPPReqFileOBIS2(tempOM, inLen, outFile);
	
err:
	return tempRet;
}

//----------------------------------------------------------------
//create sml file containing GPPReq messages for OBIS parameter
int32_t SMLF_CreateGetPPReqFileOBIS(OBISMapping_t* inOBIS, sml_file** outFile)
{
	return SMLF_CreateGetPPReqFileOBIS2(&inOBIS, 1, outFile);
}

//----------------------------------------------------------------
//create sml file containing GPPReq messages for array of OBIS parameter
int32_t SMLF_CreateGetPPReqFileOBIS2(OBISMapping_t** inOBISArray, uint16_t inLen, sml_file** outFile)
{
	int32_t tempRet = SMLF_E_Unknown;
	sml_file* tempF = NULL;
	
	ASSERT_B(inOBISArray && inLen > 0 && outFile, SMLF_E_ParamInput);
	
	tempF = sml_file_init(0);
	ASSERT_B(tempF, SMLF_E_Malloc);
	
	uint32_t tempReqID = Misc_SwapByteOrder32(_ReqFileID++);
	sml_message* tempOR = SMLP_Message_GenerateOpenRequest(_SML_Server_ID, sizeof(_SML_Server_ID), (uint8_t*)&tempReqID, sizeof(tempReqID));
	ASSERT_B(tempOR, SMLF_E_Malloc);
	
	sml_file_add_message(tempF, tempOR);
	
	for (uint16_t i = 0; i < inLen; i++)
	{
		if (!inOBISArray[i] || !(inOBISArray[i]->Flags & PF_R)) continue;
		
		sml_message* tempM = NULL;
		ASSERT_F(_SMLF_CreateGetPPReqMessage(inOBISArray[i], &tempM));
		
		sml_file_add_message(tempF, tempM);
	}
	
	sml_message* tempCR = SMLP_Message_GenerateCloseRequest();
	ASSERT_B(tempCR, SMLF_E_Malloc);
	
	sml_file_add_message(tempF, tempCR);
	
	ASSERT_F(SMLP_File_Verify(tempF));
	
	//serialize 2 buffer
	sml_transport_write(tempF);
	*outFile = tempF;
	
	return SMLF_OK;
	
err:
	sml_file_free(tempF);
	return tempRet;
}

//----------------------------------------------------------------
//create get proc parameter req message
int32_t _SMLF_CreateGetPPReqMessage(OBISMapping_t* inOM, sml_message** outM)
{
	int32_t tempRet = SMLF_E_Unknown;
	sml_message* tempR = NULL;
	sml_tree_path* tempTP = NULL;
	
	ASSERT_B(inOM && outM, SMLF_E_ParamInput);
	
	tempTP = sml_tree_path_init();
	ASSERT_B(tempTP, SMLF_E_Malloc);
			
	uint8_t tempB[LEN_OBIS_CLASS_VERS] = { 0 };
	memcpy(tempB, inOM->OBIS, LEN_OBIS);
	*(uint16_t*)(tempB + LEN_OBIS) = Misc_SwapByteOrder16(inOM->COSEMClass);  //set class
	tempB[LEN_OBIS + 2] = 0;   //all class versions 0
	
	ASSERT_B(sml_tree_path_add_path_entry(tempTP, sml_octet_string_init(tempB, sizeof(tempB))) == SML_OK, SMLF_E_Malloc);
	
#define _M_LOOP(_i)  do{for(uint16_t i=2; i<=_i; i++) { uint16_t ar = Misc_SwapByteOrder16(i); ASSERT_B(sml_tree_path_add_path_entry(tempTP, sml_octet_string_init((uint8_t*)&ar, 2)) == SML_OK, SMLF_E_Malloc); }} while(0)
	
	switch (inOM->COSEMClass)
	{
	case COSC_Data: //2 data
		{
			_M_LOOP(2);
			break;
		}
	case COSC_Reg: //2 data, 3 scaler_unit
		{
			_M_LOOP(3);
			break;
		}
	case COSC_AdvExtReg: //2 data, 3 scaler_unit, 4 capture time, 5 status
		{
			_M_LOOP(5);
			break;
		}
	default:
		ASSERT_F(SMLF_E_COSEMClassType);
	}
	
#undef _M_LOOP
	
	tempR = SMLP_Message_GenerateGetProcParameterRequest(_SML_Server_ID, sizeof(_SML_Server_ID), tempTP);
	ASSERT_B(tempR, SMLF_E_Malloc);
	*outM = tempR;
	
	return SMLF_OK;
	
err:
	sml_tree_path_free(tempTP);
	sml_message_free(tempR);
	return tempRet;
}

//----------------------------------------------------------------
//create sml file containing SPPReq messages for PID parameter
int32_t SMLF_CreateSetPPReqFilePID(ParamID_t inPID, uCOSEMValue_t* inData, sml_file** outFile)
{
	OBISDataChain_t tempOD = { .Data = *inData, .Mapping = OBIS_MappingGet(inPID) };
	
	return SMLF_CreateSetPPReqFileOBIS(&tempOD, outFile);
}

//----------------------------------------------------------------
//create sml file containing SPPReq messages for OBIS data
int32_t SMLF_CreateSetPPReqFileOBIS(OBISDataChain_t* inData, sml_file** outFile)
{
	int32_t tempRet = SMLF_E_Unknown;
	sml_file* tempF = NULL;
	
	ASSERT_B(inData && inData->Mapping && outFile, SMLF_E_ParamInput);
	ASSERT_B(inData->Mapping->Flags & PF_W, SMLF_E_ParamInput);
	
	tempF = sml_file_init(0);
	ASSERT_B(tempF, SMLF_E_Malloc);
	
	uint32_t tempReqID = Misc_SwapByteOrder32(_ReqFileID++);
	sml_message* tempOR = SMLP_Message_GenerateOpenRequest(_SML_Server_ID, sizeof(_SML_Server_ID), (uint8_t*)&tempReqID, sizeof(tempReqID));
	ASSERT_B(tempOR, SMLF_E_Malloc);
	
	sml_file_add_message(tempF, tempOR);
	
	sml_message* tempM = NULL;
	ASSERT_F(_SMLF_CreateSetPPReqMessage(inData->Mapping, &inData->Data.CData.Value, &tempM));
		
	sml_file_add_message(tempF, tempM);
	
	sml_message* tempCR = SMLP_Message_GenerateCloseRequest();
	ASSERT_B(tempCR, SMLF_E_Malloc);
	
	sml_file_add_message(tempF, tempCR);
	
	ASSERT_F(SMLP_File_Verify(tempF));
	
	//serialize 2 buffer
	sml_transport_write(tempF);
	*outFile = tempF;
	
	return SMLF_OK;
	
err:
	sml_file_free(tempF);
	return tempRet;
}

//----------------------------------------------------------------
//create set proc parameter req message
int32_t _SMLF_CreateSetPPReqMessage(OBISMapping_t* inOM, uCOSEMValue_t* inValue, sml_message** outM)
{
	int32_t tempRet = SMLF_E_Unknown;
	sml_message* tempR = NULL;
	sml_tree* tempT = NULL;
	sml_tree_path* tempTP = NULL;
	
	ASSERT_B(inOM && inValue && outM, SMLF_E_ParamInput);
	ASSERT_B(inOM->COSEMClass == COSC_Data, SMLF_E_COSEMClassType); //only COSEM class type data
	
	tempT = sml_tree_init();
	ASSERT_B(tempT, SMLF_E_Malloc);
	
	tempTP = sml_tree_path_init();
	ASSERT_B(tempTP, SMLF_E_Malloc);
			
	uint8_t tempB[LEN_OBIS_CLASS_VERS] = { 0 };
	memcpy(tempB, inOM->OBIS, LEN_OBIS);
	*(uint16_t*)(tempB + LEN_OBIS) = Misc_SwapByteOrder16(inOM->COSEMClass);   //set class
	tempB[LEN_OBIS + 2] = 0;    //all class versions 0
	
	tempT->parameter_name = sml_octet_string_init(tempB, sizeof(tempB));
	ASSERT_B(tempT->parameter_name, SMLF_E_Malloc);
	ASSERT_B(sml_tree_path_add_path_entry(tempTP, sml_octet_string_init(tempB, sizeof(tempB))) == SML_OK, SMLF_E_Malloc);
	
	//attrib 2 data
	uint16_t ar = Misc_SwapByteOrder16(2); 
	
	//param tree path
	ASSERT_B(sml_tree_path_add_path_entry(tempTP, sml_octet_string_init((uint8_t*)&ar, 2)) == SML_OK, SMLF_E_Malloc);
	
	//param tree
	ASSERT_B(sml_tree_add_tree(tempT, sml_tree_init()) == SML_OK, SMLF_E_Malloc);
	sml_tree* tempCld = tempT->child_list[0];
	tempCld->parameter_name = sml_octet_string_init((uint8_t*)&ar, 2);
	ASSERT_B(tempCld->parameter_name, SMLF_E_Malloc);
	
	//param tree value
	tempCld->parameter_value = sml_proc_par_value_init();
	ASSERT_B(tempCld->parameter_value, SMLF_E_Malloc);
	tempCld->parameter_value->tag = sml_u8_init(SML_PROC_PAR_VALUE_TAG_VALUE);
	ASSERT_B(tempCld->parameter_value->tag, SMLF_E_Malloc);
	tempCld->parameter_value->data.value = sml_value_init();
	sml_value* tempV = tempCld->parameter_value->data.value;
	ASSERT_B(tempV, SMLF_E_Malloc);
	tempV->type = inOM->PType;
	
	if (inOM->PType == PT_Bool)
	{
		tempV->data.boolean = sml_boolean_init(inValue->Bool);
	}
	else if (inOM->PType == PT_String)
	{
		ASSERT_B(inValue->Bytes.Array && inValue->Bytes.Len >= inOM->LenMin && inValue->Bytes.Len <= inOM->LenMax, SMLF_E_ParamInput);
		tempV->data.bytes = sml_octet_string_init(inValue->Bytes.Array, inValue->Bytes.Len);
		
	}
	else
		ASSERT_F(SMLF_E_ParamInput); //only bool and string
	
	tempR = SMLP_Message_GenerateSetProcParameterRequest(_SML_Server_ID, sizeof(_SML_Server_ID), tempTP, tempT);
	ASSERT_B(tempR, SMLF_E_Malloc);
	*outM = tempR;
	
	return SMLF_OK;
	
err:
	sml_tree_free(tempT);
	sml_tree_path_free(tempTP);
	sml_message_free(tempR);
	return tempRet;
}

//--------------------------------
//sml file parsing

//----------------------------------------------------------------
//parse sml file containing SPPResp messages
int32_t SMLF_ParseSetPPRespFile(sml_file* inFile, uint64_t* outATTNNr)
{
	int32_t tempRet = SMLF_E_Unknown;
	
	ASSERT_B(inFile, SMLF_E_ParamInput);
	ASSERT_F(SMLP_File_Verify(inFile));
	ASSERT_B(inFile->messages_len == 3, SMLF_E_SMLFileStructure);
	
	sml_message* tempM = inFile->messages[1];
	switch (*tempM->message_body->tag)
	{
	case SML_MESSAGE_ATTENTION_RESPONSE:
		{
			sml_attention_response* tempSpec = tempM->message_body->d.attention_response;
			ASSERT_B((TEST_P(tempSpec, attention_number, str)) && tempSpec->attention_number->len == 6, SMLF_E_ValueTypeMismatch);
			
			uint64_t tempA = 0;
			memcpy(&tempA, tempSpec->attention_number->str, 6);
			tempA = (((uint64_t)Misc_SwapByteOrder32(tempA)) << 32) | ((uint64_t)Misc_SwapByteOrder32(tempA >> 32));
			tempA >>= 16;
			
			if (outATTNNr) *outATTNNr = tempA;
			ASSERT_B(tempA == (SML_ATTN_PREFIX | SML_ATTN_Ok), SMLF_E_SMLAttention);
			
			break;
		}
	default:
		{
			TRACE(LMN_GetErrorMessage(SMLF_E_SMLMessageType));
			break;
		}
	}
	
	return SMLF_OK;
	
err:
	return tempRet;
}

//----------------------------------------------------------------
//parse sml file containing GPPResp messages
int32_t SMLF_ParseGetPPRespFile(sml_file* inFile, OBISDataChain_t** outData)
{
	int32_t tempRet = SMLF_E_Unknown;
	
	ASSERT_B(inFile && outData, SMLF_E_ParamInput);
	ASSERT_F(SMLP_File_Verify(inFile));
	
	for (uint16_t i = 1; i < inFile->messages_len - 1; i++)
	{
		sml_message* tempM = inFile->messages[i];
		
		switch (*tempM->message_body->tag)
		{
		case SML_MESSAGE_ATTENTION_RESPONSE:
			{
				TRACE(LMN_GetErrorMessage(SMLF_E_SMLAttention));
				
				sml_attention_response* tempSpec = tempM->message_body->d.attention_response;
				break;
			}
		case SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE:
			{
				sml_get_proc_parameter_response* tempSpec = tempM->message_body->d.get_proc_parameter_response;
				int32_t tempR = _SMLF_ParseGetPPRespMessage(tempSpec, outData);
				if (tempR != SMLF_OK) TRACE(LMN_GetErrorMessage(tempR));
				break;
			}
		default:
			{
				TRACE(LMN_GetErrorMessage(SMLF_E_SMLMessageType));
				break;
			}
		}
	}
	
	return SMLF_OK;
	
err:
	return tempRet;
}

//----------------------------------------------------------------
//parse GPPResp sml message
int32_t _SMLF_ParseGetPPRespMessage(sml_get_proc_parameter_response* inMsg, OBISDataChain_t** outData)
{
	int32_t tempRet = SMLF_E_Unknown;
	
	ASSERT_B(inMsg && outData, SMLF_E_ParamInput);
	
	ASSERT_B((TEST_P(inMsg, parameter_tree, parameter_name, str)) && inMsg->parameter_tree->parameter_name->len == LEN_OBIS_CLASS_VERS, SMLF_E_SMLGPPRespMalformed);
	uint8_t* tempOBIS = inMsg->parameter_tree->parameter_name->str;
	
	OBISMapping_t* tempOM = OBIS_MappingGet2(tempOBIS);
	ASSERT_B(tempOM, SMLF_E_OBISUnknown);
	
	uint16_t tempClass = Misc_SwapByteOrder16(*((uint16_t*)&tempOBIS[LEN_OBIS]));
	uint8_t tempVers = tempOBIS[LEN_OBIS + 2];
	
	ASSERT_B(tempClass == tempOM->COSEMClass && tempVers == 0, SMLF_E_COSEMClassProperties);
	
	switch (tempClass)
	{
	case COSC_Data: //2 data
		{
			ASSERT_F(_SMLF_ParseCOSEMClass_1(inMsg->parameter_tree->child_list, inMsg->parameter_tree->child_list_len, tempOM, outData));
			break;
		}
	case COSC_Reg: //2 data, 3 scaler_unit
		{
			ASSERT_F(_SMLF_ParseCOSEMClass_3(inMsg->parameter_tree->child_list, inMsg->parameter_tree->child_list_len, tempOM, outData));
			break;
		}
	case COSC_AdvExtReg: //2 data, 3 scaler_unit, 4 capture time, 5 status
		{
			ASSERT_F(_SMLF_ParseCOSEMClass_32770(inMsg->parameter_tree->child_list, inMsg->parameter_tree->child_list_len, tempOM, outData));
			break;
		}
	default:
		ASSERT_F(SMLF_E_COSEMClassType);
	}
	
	return SMLF_OK;
	
err:
	return tempRet;
}

//----------------------------------------------------------------
//parse GPPResp sml message for COSEM data class
int32_t _SMLF_ParseCOSEMClass_1(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData)
{
	int32_t tempRet = SMLF_E_Unknown;
	OBISDataChain_t* tempD = NULL;
	
	ASSERT_B(inTree && inTreeLen > 0 && inOM && outData, SMLF_E_ParamInput);
	
	tempD = malloc(sizeof(OBISDataChain_t));
	ASSERT_B(tempD, SMLF_E_Malloc);
	memset(tempD, 0, sizeof(OBISDataChain_t));
	tempD->Mapping = inOM;

	for (uint16_t i = 0; i < inTreeLen; i++)
	{
		ASSERT_B((TEST_P(inTree[i], parameter_name, str)) && inTree[i]->parameter_name->len == 2, SMLF_E_SMLGPPRespMalformed);
		uint16_t tempAttrIdx = Misc_SwapByteOrder16(*((uint16_t*)inTree[i]->parameter_name->str));
		
		sml_proc_par_value* tempPPV = inTree[i]->parameter_value;
		ASSERT_B(tempPPV && tempPPV->tag, SMLF_E_SMLGPPRespMalformed);
		
		switch (tempAttrIdx)
		{
		case 2://data
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;
				
				ASSERT_F(_SMLF_TxferSMLValue2OBISData(inOM, tempV, &tempD->Data.CReg.Value));
				
				break;
			}
		default:
			ASSERT_F(SMLF_E_COSEMClassProperties);
		}	
	}
	
	//insert data @ chain start
	tempD->Next = *outData;
	*outData = tempD;
	
	return SMLF_OK;
	
err:
	OBIS_DataFreeSingle(tempD);
	return tempRet;
}

//----------------------------------------------------------------
//parse GPPResp sml message for COSEM register class
int32_t _SMLF_ParseCOSEMClass_3(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData)
{
	int32_t tempRet = SMLF_E_Unknown;
	OBISDataChain_t* tempD = NULL;
	
	ASSERT_B(inTree && inTreeLen > 0 && inOM && outData, SMLF_E_ParamInput);
	
	tempD = malloc(sizeof(OBISDataChain_t));
	ASSERT_B(tempD, SMLF_E_Malloc);
	memset(tempD, 0, sizeof(OBISDataChain_t));
	tempD->Mapping = inOM;

	for (uint16_t i = 0; i < inTreeLen; i++)
	{
		ASSERT_B((TEST_P(inTree[i], parameter_name, str)) && inTree[i]->parameter_name->len == 2, SMLF_E_SMLGPPRespMalformed);
		uint16_t tempAttrIdx = Misc_SwapByteOrder16(*((uint16_t*)inTree[i]->parameter_name->str));
		
		sml_proc_par_value* tempPPV = inTree[i]->parameter_value;
		ASSERT_B(tempPPV && tempPPV->tag, SMLF_E_SMLGPPRespMalformed);
		
		switch (tempAttrIdx)
		{
		case 2://data
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;

				ASSERT_F(_SMLF_TxferSMLValue2OBISData(inOM, tempV, &tempD->Data.CReg.Value));
				
				break;
			}
		case 3://scaler_unit
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;
				ASSERT_B(tempV && tempV->type == SML_TYPE_LIST, SMLF_E_ValueTypeMismatch);
				sml_list_type* tempL = tempV->data.smlList;
				ASSERT_B(tempL && tempL->tag && *tempL->tag == SML_LIST_TYPE_COSEM_VALUE, SMLF_E_ValueTypeMismatch);
				sml_cosem_value *tempCV = tempL->data.cosem_value;
				ASSERT_B(tempCV && tempCV->tag && *tempCV->tag == SML_COSEM_VALUE_SCALER_UNIT, SMLF_E_ValueTypeMismatch);
				sml_cosem_scaler_unit *tempSU = tempCV->data.scaler_unit;
				ASSERT_B(tempSU && tempSU->scaler && tempSU->unit, SMLF_E_ValueTypeMismatch);
				
				tempD->Data.CReg.Scaler = *tempSU->scaler;
				tempD->Data.CReg.Unit = *tempSU->unit;
				
				break;
			}
		default:
			ASSERT_F(SMLF_E_COSEMClassProperties);
		}	
	}
	
	//insert data @ chain start
	tempD->Next = *outData;
	*outData = tempD;
	
	return SMLF_OK;
	
err:
	OBIS_DataFreeSingle(tempD);
	return tempRet;
}

//----------------------------------------------------------------
//parse GPPResp sml message for COSEM advanced ext register class
int32_t _SMLF_ParseCOSEMClass_32770(struct s_tree** inTree, uint16_t inTreeLen, OBISMapping_t* inOM, OBISDataChain_t** outData)
{
	int32_t tempRet = SMLF_E_Unknown;
	OBISDataChain_t* tempD = NULL;
	
	ASSERT_B(inTree && inTreeLen > 0 && inOM && outData, SMLF_E_ParamInput);
	
	tempD = malloc(sizeof(OBISDataChain_t));
	ASSERT_B(tempD, SMLF_E_Malloc);
	memset(tempD, 0, sizeof(OBISDataChain_t));
	tempD->Mapping = inOM;

	for (uint16_t i = 0; i < inTreeLen; i++)
	{
		ASSERT_B((TEST_P(inTree[i], parameter_name, str)) && inTree[i]->parameter_name->len == 2, SMLF_E_SMLGPPRespMalformed);
		uint16_t tempAttrIdx = Misc_SwapByteOrder16(*((uint16_t*)inTree[i]->parameter_name->str));
		
		sml_proc_par_value* tempPPV = inTree[i]->parameter_value;
		ASSERT_B(tempPPV && tempPPV->tag, SMLF_E_SMLGPPRespMalformed);
		
		switch (tempAttrIdx)
		{
		case 2://data
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;

				ASSERT_F(_SMLF_TxferSMLValue2OBISData(inOM, tempV, &tempD->Data.CAdvExtReg.Value));
				
				break;
			}
		case 3://scaler_unit
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;
				ASSERT_B(tempV && tempV->type == SML_TYPE_LIST, SMLF_E_ValueTypeMismatch);
				sml_list_type* tempL = tempV->data.smlList;
				ASSERT_B(tempL && tempL->tag && *tempL->tag == SML_LIST_TYPE_COSEM_VALUE, SMLF_E_ValueTypeMismatch);
				sml_cosem_value *tempCV = tempL->data.cosem_value;
				ASSERT_B(tempCV && tempCV->tag && *tempCV->tag == SML_COSEM_VALUE_SCALER_UNIT, SMLF_E_ValueTypeMismatch);
				sml_cosem_scaler_unit *tempSU = tempCV->data.scaler_unit;
				ASSERT_B(tempSU && tempSU->scaler && tempSU->unit, SMLF_E_ValueTypeMismatch);
				
				tempD->Data.CAdvExtReg.Scaler = *tempSU->scaler;
				tempD->Data.CAdvExtReg.Unit = *tempSU->unit;
				
				break;
			}
		case 4://capture time
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;
				ASSERT_B((tempV->type & 0xfffffff0) == SML_TYPE_UNSIGNED, SMLF_E_ValueTypeMismatch);
				
				uCOSEMValue_t tempCV = { 0 };
				ASSERT_F(_SMLF_TxferSMLValue2OBISData(NULL, tempV, &tempCV));
				
				tempD->Data.CAdvExtReg.CaptureTime = tempCV.UInt32;
				
				break;
			}
		case 5://status
			{
				ASSERT_B(*tempPPV->tag == SML_PROC_PAR_VALUE_TAG_VALUE, SMLF_E_ValueTypeMismatch);
				sml_value* tempV = tempPPV->data.value;
				ASSERT_B((tempV->type & 0xfffffff0) == SML_TYPE_UNSIGNED, SMLF_E_ValueTypeMismatch);
				
				uCOSEMValue_t tempCV = { 0 };
				ASSERT_F(_SMLF_TxferSMLValue2OBISData(NULL, tempV, &tempCV));
				
				tempD->Data.CAdvExtReg.Status = tempCV.UInt32;
				
				break;
			}
		default:
			ASSERT_F(SMLF_E_COSEMClassProperties);
		}	
	}
	
	//insert data @ chain start
	tempD->Next = *outData;
	*outData = tempD;
	
	return SMLF_OK;
	
err:
	OBIS_DataFreeSingle(tempD);
	return tempRet;
}

//----------------------------------------------------------------
//txfer data value from sml_value to COSEMValue
//mapping optional to test for expectet type
int32_t _SMLF_TxferSMLValue2OBISData(OBISMapping_t* inOM, sml_value* inSMLValue, uCOSEMValue_t* inCValue)
{
	int32_t tempRet = SMLF_E_Unknown;
	
	ASSERT_B(inSMLValue && inCValue, SMLF_E_ParamInput);
	
	if (inSMLValue->type == PT_Bool)
	{
		if (inOM) ASSERT_B(inOM->PType == PT_Bool, SMLF_E_ValueTypeMismatch);
		
		inCValue->Bool = *inSMLValue->data.boolean == 0x01;
	}
	else if (inSMLValue->type == PT_String)
	{
		if (inOM) ASSERT_B(inOM->PType == PT_String, SMLF_E_ValueTypeMismatch);
		
		ASSERT_B(inSMLValue->data.bytes && inSMLValue->data.bytes->str && inSMLValue->data.bytes->len > 0, SMLF_E_SMLValueMalformed);
		
		//constrain len
		uint16_t tempLenM = inSMLValue->data.bytes->len;
		if (inOM) tempLenM = tempLenM < inOM->LenMin ? inOM->LenMin : tempLenM > inOM->LenMax ? inOM->LenMax : tempLenM; //constrain lenght
		uint16_t tempLenC = inSMLValue->data.bytes->len;
		tempLenC = tempLenC < tempLenM ? tempLenC : tempLenM;
		
		inCValue->Bytes.Array = malloc(tempLenM);
		ASSERT_B(inCValue->Bytes.Array, SMLF_E_Malloc);
		memset(inCValue->Bytes.Array, 0, tempLenM);
		memcpy(inCValue->Bytes.Array, inSMLValue->data.bytes->str, tempLenC);
		inCValue->Bytes.Len = tempLenM;
	}
	else if ((inSMLValue->type & 0xfffffff0) == SML_TYPE_UNSIGNED)//uint
	{
		if (inOM) ASSERT_B((inOM->PType & 0xfffffff0) == SML_TYPE_UNSIGNED, SMLF_E_ValueTypeMismatch); 	//lenght test not required hmm???
		
		ASSERT_B(inSMLValue->data.uint64, SMLF_E_SMLValueMalformed);
		
		uint8_t tempLen = inSMLValue->type & 0x0f;
		
		memcpy(&inCValue->UInt64, inSMLValue->data.uint64, tempLen);
	}
	else if((inSMLValue->type & 0xfffffff0) == SML_TYPE_INTEGER)//int
	{
		if (inOM) ASSERT_B((inOM->PType & 0xfffffff0) == SML_TYPE_INTEGER, SMLF_E_ValueTypeMismatch);   //lenght test not required hmm???
		
		ASSERT_B(inSMLValue->data.int64, SMLF_E_SMLValueMalformed);
		
		uint8_t tempLen = inSMLValue->type & 0x0f;
		int8_t tempI8 = *inSMLValue->data.int64 >> (8*(tempLen - 1));
		
		if (tempI8 < 0) inCValue->Int64 = -1;
		
		memcpy(&inCValue->Int64, inSMLValue->data.int64, tempLen);
	}
	
	return SMLF_OK;
	
err:
	return tempRet;
}
