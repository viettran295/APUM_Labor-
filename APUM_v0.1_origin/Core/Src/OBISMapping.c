#include "OBISMapping.h"

const OBISMapping_t OBIS_Mapping[] = 
{
	//ID						OBIS								COSEM class			type				len min		len max		flags
	{PID_SecIndex,				"\x01\x00\x5E\x31\x00\x01",			COSC_Data,			PT_UInt32,			4,			4,			PF_R},		//Sekundenindex
	{PID_InfoAct,				"\x01\x00\x5E\x31\x01\x02",			COSC_Data,			PT_Bool,			1,			1,			PF_RW},		//Aktivierung der Info-schnittstelle
	{PID_MagnManipulationCnt,	"\x01\x00\x5E\x31\x01\x03",			COSC_Data,			PT_UInt16,			2,			2,			PF_R},		//Counter magnetische Manipulation
	{PID_MagnManipulationAct,	"\x01\x00\x5E\x31\x01\x04",			COSC_Data,			PT_Bool,			1,			1,			PF_RW}, 	//Aktivierung magnetische Manipulation
	{PID_MechManipulationCnt,	"\x01\x00\x5E\x31\x01\x0A",			COSC_Data,			PT_UInt16,			2,			2,			PF_R}, 		//Counter magnetische Manipulation
	{PID_MechManipulationAct,	"\x01\x00\x5E\x31\x01\x09",			COSC_Data,			PT_Bool,			1,			1,			PF_RW},  	//Aktivierung magnetische Manipulation
	
	{PID_DeviceClass,			"\x01\x00\x5E\x31\x01\x05",			COSC_Data,			PT_String,			6,			6,			PF_R},		//Geraeteklasse
	{PID_ManufacturerID,		"\x01\x00\x60\x32\x01\x01",			COSC_Data,			PT_String,			1,			4,			PF_R},		//Herstellerkennung
	{PID_DeviceID,				"\x01\x00\x60\x01\x00\xFF",			COSC_Data,			PT_String,			10,			10,			PF_R},		//Geraetekennung
	{PID_DeviceFirmwareVers,	"\x01\x00\x00\x02\x00\x00",			COSC_Data,			PT_String,			1,			8,			PF_R},		//Version Geraetefirmware
	{PID_DeviceFirmwareCRC,		"\x01\x00\x60\x5A\x02\x01",			COSC_Data,			PT_String,			1,			8,			PF_R},		//CRC Geraetefirmware
	{PID_CryptoReset,			"\x01\x00\x5E\x31\x00\x07",			COSC_Data,			PT_Bool,			1,			1,			PF_W},		//Cryptoreset
	
	{PID_APlus,					"\x01\x00\x01\x08\x00\xFF",			COSC_AdvExtReg,		PT_UInt64,			8,			8,			PF_R},		//Zaehlerstand +A
	{PID_AMinus,				"\x01\x00\x02\x08\x00\xFF",			COSC_AdvExtReg,		PT_UInt64,			8,			8,			PF_R}, 		//Zaehlerstand -A
	{PID_Voltage_L1,			"\x01\x00\x20\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R},		//Spannung L1
	{PID_Voltage_L2,			"\x01\x00\x34\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R}, 		//Spannung L2
	{PID_Voltage_L3,			"\x01\x00\x48\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R}, 		//Spannung L3
	{PID_Current_L1,			"\x01\x00\x1F\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R}, 		//Strom L1
	{PID_Current_L2,			"\x01\x00\x33\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R}, 		//Strom L2
	{PID_Current_L3,			"\x01\x00\x47\x07\x00\xFF",			COSC_Reg,			PT_UInt64,			8,			8,			PF_R}, 		//Strom L3

	{PID_PowerSum,				"\x01\x00\x10\x07\x00\xFF",			COSC_Reg,			PT_Int64,			8,			8,			PF_R},		//momentane Wirkleistung aller Phasen
	
	{PID_Status,				"\x01\x00\x60\x05\x00\xFF",			COSC_Data,			PT_UInt32,			4,			4,			PF_R},		//Statuswort
};

//extern count of obisMapping
const uint16_t OBIS_MappingCnt = sizeof(OBIS_Mapping) / sizeof(OBISMapping_t);

//----------------------------------------------------------------
//get mapping for PID
OBISMapping_t* OBIS_MappingGet(ParamID_t inPID)
{
	for (uint16_t i = 0; i < OBIS_MappingCnt; i++)
		if (OBIS_Mapping[i].PID == inPID) return (OBISMapping_t*)&OBIS_Mapping[i];
	
	return NULL;
}

//----------------------------------------------------------------
//get mapping for OBIS
OBISMapping_t* OBIS_MappingGet2(uint8_t* inOBIS)
{
	if (!inOBIS) return NULL;
	
	for (uint16_t i = 0; i < OBIS_MappingCnt; i++)
		if (!memcmp(OBIS_Mapping[i].OBIS, inOBIS, sizeof(OBIS_Mapping[i].OBIS))) return (OBISMapping_t*)&OBIS_Mapping[i];
	
	return NULL;
}
