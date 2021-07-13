#include "OBIS_Data.h"
#include <FreeRTOS.h>
#include <string.h>

#define malloc pvPortMalloc
#define free vPortFree
#define realloc pvPortRealloc

//----------------------------------------------------------------
//free single data chain entry
void OBIS_DataFreeSingle(OBISDataChain_t* inEntry)
{
	if (!inEntry) return;
	
	//string?
	if(inEntry->Mapping && inEntry->Mapping->PType == PT_String)
	{
		switch (inEntry->Mapping->COSEMClass)
		{
		case COSC_Data:
			{
				if (inEntry->Data.CData.Value.Bytes.Array)
					free(inEntry->Data.CData.Value.Bytes.Array);
				break;
			}
		case COSC_Reg:
			{
				if (inEntry->Data.CReg.Value.Bytes.Array)
					free(inEntry->Data.CReg.Value.Bytes.Array);
				break;
			}
		case COSC_AdvExtReg:
			{
				if (inEntry->Data.CAdvExtReg.Value.Bytes.Array)
					free(inEntry->Data.CAdvExtReg.Value.Bytes.Array);
				break;
			}
		default:
			break;
		}
	}

	memset(inEntry, 0, sizeof(OBISDataChain_t)); //erase, just to be sure??
	free(inEntry);
}

//----------------------------------------------------------------
//free entire data chain
void OBIS_DataFreeChain(OBISDataChain_t* inChain)
{
	while (inChain)
	{
		OBISDataChain_t* tempN = inChain->Next;
		OBIS_DataFreeSingle(inChain);
		inChain = tempN;
	}
}

//----------------------------------------------------------------
//find element in chain by PID
OBISDataChain_t* OBIS_DataFindByPID(OBISDataChain_t* inChain, ParamID_t inPID)
{
	OBISMapping_t* tempM = OBIS_MappingGet(inPID);
	return OBIS_DataFindByMapping(inChain, tempM);
}

//----------------------------------------------------------------
//find element in chain by OBIS
OBISDataChain_t* OBIS_DataFindByOBIS(OBISDataChain_t* inChain, uint8_t* inOBIS)
{
	OBISMapping_t* tempM = OBIS_MappingGet2(inOBIS);
	return OBIS_DataFindByMapping(inChain, tempM);
}

//----------------------------------------------------------------
//find element in chain by mapping
OBISDataChain_t* OBIS_DataFindByMapping(OBISDataChain_t* inChain, OBISMapping_t* inMapping)
{
	while (inChain)
	{
		if (inChain->Mapping == inMapping)
			return inChain;
		
		inChain = inChain->Next;
	}
	
	return NULL;
}