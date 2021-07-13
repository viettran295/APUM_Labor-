#ifndef OBIS_DATA_H
#define OBIS_DATA_H

#include "OBISMapping.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct
{
	uint64_t Reserved; //avoid overlapping with native types @ uCOSEMValue_t
	uint8_t* Array;
	uint16_t Len;
}strByteArray_t;

typedef union
{
	bool			Bool;
	int8_t			Int8;
	int16_t			Int16;
	int32_t			Int32;
	int64_t			Int64;
	uint8_t			UInt8;
	uint16_t		UInt16;
	uint32_t		UInt32;
	uint64_t		UInt64;
	strByteArray_t	Bytes;
} uCOSEMValue_t;

typedef struct
{
	uCOSEMValue_t Value;
}strCOSEMData_t;

typedef struct
{
	uCOSEMValue_t Value;
	int8_t Scaler;
	ParamUnit_t Unit;
}strCOSEMReg_t;

typedef struct
{
	uCOSEMValue_t Value;
	int8_t Scaler;
	ParamUnit_t Unit;
	uint32_t CaptureTime;
	uint32_t Status;
}strCOSEM_AdvExtReg_t;

typedef union
{
	strCOSEMData_t CData;
	strCOSEMReg_t CReg;
	strCOSEM_AdvExtReg_t CAdvExtReg;
}uCOSEMClass_t;

typedef struct OBIS_DATA_CHAIN_T
{
	OBISMapping_t* Mapping;
	uCOSEMClass_t Data;
	struct OBIS_DATA_CHAIN_T* Next;
}OBISDataChain_t;

void OBIS_DataFreeSingle(OBISDataChain_t* inEntry);
void OBIS_DataFreeChain(OBISDataChain_t* inChain);
OBISDataChain_t* OBIS_DataFindByPID(OBISDataChain_t* inChain, ParamID_t inPID);
OBISDataChain_t* OBIS_DataFindByOBIS(OBISDataChain_t* inChain, uint8_t* inOBIS);
OBISDataChain_t* OBIS_DataFindByMapping(OBISDataChain_t* inChain, OBISMapping_t* inMapping);

#endif // !OBIS_DATA_H