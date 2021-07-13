#ifndef SML_FACTORY_H
#define SML_FACTORY_H

#include "sml/sml_file.h"
#include "OBISMapping.h"
//#include "HDLC_Meter.h"
#include <stdint.h>
#include "OBIS_Data.h"
//#include <StdRet.h>

typedef enum
{
	SMLF_OK							= 0,
	SMLF_E_Unknown					= -4001,
	SMLF_E_ParamInput				= -4002,
	SMLF_E_Malloc					= -4003,
	SMLF_E_COSEMClassType			= -4004,
	SMLF_E_COSEMClassProperties		= -4005,
	SMLF_E_SMLFileStructure			= -4006,
	SMLF_E_SMLMessageType			= -4007,
	SMLF_E_SMLAttention				= -4008,
	SMLF_E_SMLGPPRespMalformed		= -4009,
	SMLF_E_SMLValueMalformed		= -4010,
	SMLF_E_OBISUnknown				= -4011,
	SMLF_E_ValueTypeMismatch		= -4012,
}SMLF_RetVal_t;

void SMLF_FileFree(sml_file* inFile);
uint8_t* SMLF_FileGetBuf(sml_file* inFile);
uint16_t SMLF_FileGetBufLen(sml_file* inFile);
sml_file* SMLF_FileParseFromBuf(uint8_t* inData, uint16_t inLen);

int32_t SMLF_CreateGetPPReqFileOBIS(OBISMapping_t* inOBIS, sml_file** outFile);
int32_t SMLF_CreateGetPPReqFileOBIS2(OBISMapping_t** inOBISArray, uint16_t inLen, sml_file** outFile);
int32_t SMLF_CreateGetPPReqFilePID(ParamID_t inPID, sml_file** outFile);
int32_t SMLF_CreateGetPPReqFilePID2(ParamID_t* inPIDArray, uint16_t inLen, sml_file** outFile);

int32_t SMLF_ParseGetPPRespFile(sml_file* inFile, OBISDataChain_t** outData);

int32_t SMLF_CreateSetPPReqFilePID(ParamID_t inPID, uCOSEMValue_t* inData, sml_file** outFile);
int32_t SMLF_CreateSetPPReqFileOBIS(OBISDataChain_t* inData, sml_file** outFile);

int32_t SMLF_ParseSetPPRespFile(sml_file* inFile, uint64_t* outATTNNr);

#endif // !SML_FACTORY_H
