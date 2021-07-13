#ifndef OBIS_MAPPING_H
#define OBIS_MAPPING_H

#include <stdint.h>
#include "SML_Protocol.h"
#include "sml/sml_shared.h"

typedef enum
{
	//0 not defined
	PID_SecIndex = 1,
	PID_InfoAct,
	PID_MagnManipulationCnt,
	PID_MagnManipulationAct,
	PID_MechManipulationCnt,
	PID_MechManipulationAct,
	
	PID_DeviceClass,
	PID_ManufacturerID,
	PID_DeviceID,
	PID_DeviceFirmwareVers,
	PID_DeviceFirmwareCRC,
	PID_CryptoReset,
	
	PID_APlus,
	PID_AMinus,
	PID_Voltage_L1,
	PID_Voltage_L2,
	PID_Voltage_L3,
	PID_Current_L1,
	PID_Current_L2,
	PID_Current_L3,
	PID_PowerSum,
	
	PID_Status,
	
}ParamID_t;

typedef enum
{
	PT_NULL   = -1,
	PT_Bool   = SML_TYPE_BOOLEAN,
	PT_UInt8  = SML_TYPE_UNSIGNED | SML_TYPE_NUMBER_8,
	PT_UInt16 = SML_TYPE_UNSIGNED | SML_TYPE_NUMBER_16,
	PT_UInt32 = SML_TYPE_UNSIGNED | SML_TYPE_NUMBER_32,
	PT_UInt64 = SML_TYPE_UNSIGNED | SML_TYPE_NUMBER_64,
	PT_Int8   = SML_TYPE_INTEGER | SML_TYPE_NUMBER_8,
	PT_Int16  = SML_TYPE_INTEGER | SML_TYPE_NUMBER_16,
	PT_Int32  = SML_TYPE_INTEGER | SML_TYPE_NUMBER_32,
	PT_Int64  = SML_TYPE_INTEGER | SML_TYPE_NUMBER_64,
	PT_String = SML_TYPE_OCTET_STRING,
} ParamType_t;

typedef enum
{
	PF_None = 0,
	PF_R    = 0x01,
	PF_W    = 0x02,
	PF_X    = 0x04,
	PF_RW   = PF_R | PF_W,
} ParamFlags_t;

typedef enum
{
	//1-10
	COSU_Year = 1,
	COSU_Month,
	COSU_Week,
	COSU_Day,
	COSU_Hour,
	COSU_Minute,
	COSU_Second,
	COSU_Degree,
	COSU_Deg_Celsius,
	COSU_Currency_loc,
	
	//11-20
	COSU_Metre,
	COSU_Meter_p_Sec,
	COSU_Cubic_Metre,
	COSU_Cubic_Metre_corr,
	COSU_Cubic_Metre_p_Hour,
	COSU_Cubic_Metre_p_Hour_corr,
	COSU_Cubic_Metre_p_Day,
	COSU_Cubic_Metre_p_Day_corr,
	COSU_Litre,
	COSU_Kilogram,
	
	//21-30
	COSU_Newton,
	COSU_Newtonmetre,
	COSU_Pascal,
	COSU_Bar,
	COSU_Joule,
	COSU_Joule_p_Hour,
	COSU_Watt,
	COSU_Volt_Ampere_apparent,
	COSU_Volt_Ampere_reactive,
	COSU_Watt_Hour,
	
	//31-40
	COSU_Volt_Ampere_Hour_apparent,
	COSU_Volt_Ampere_Hour_reactive,
	COSU_Ampere,
	COSU_Coulomb,
	COSU_Volt,
	COSU_Volt_p_Metre,
	COSU_Farad,
	COSU_Ohm,
	COSU_Ohm_Square_Metre_p_Metre,
	COSU_Weber,
	
	//41-50
	COSU_Tesla,
	COSU_Ampere_p_Metre,
	COSU_Henry,
	COSU_Hertz,
	COSU_1_p_Watt_Hour,
	COSU_1_p_Volt_Ampere_Hour_reactive,
	COSU_1_p_Volt_Ampere_Hour_apparent,
	COSU_Square_Volt_Hour,
	COSU_Square_Ampere_Hour,
	COSU_Kilogram_Second,

	//51-60
	COSU_Siemens,
	COSU_Kelvin,
	COSU_1_p_Square_Volt_Hour,
	COSU_1_p_Square_Ampere_Hour,
	COSU_1_p_Cubic_Metre,
	COSU_Percent,
	COSU_Ampere_Hour,
	//...
	COSU_Watt_Hour_p_Qubic_Metre = 60,
	
	//61-70
	COSU_Joule_p_Qubic_Metre,
	COSU_Mole_Percent,
	COSU_Gram_p_Qubic_Metre,
	COSU_Pascal_Second,
	COSU_Joule_p_Kilogram,
	//...
	COSU_Decibel_Milliwatts = 70,
	
	//other
	COSU_Other = 254,
	COSU_None = 255,
		
} ParamUnit_t;

typedef enum
{
	COSC_Data = 1,
	COSC_Reg = 3,
	//	COSC_SignExtReg      = 32768,
	//	COSC_SignExtRegTuple = 32769,
		COSC_AdvExtReg = 32770,
} COSEM_Class_t;

typedef struct
{
	ParamID_t		PID;  			//parameter-ID
	uint8_t			OBIS[LEN_OBIS];  //OBIS-Code
	COSEM_Class_t	COSEMClass;  	//COSEM class ID
	ParamType_t		PType;  			//parameter type
	uint16_t		LenMin;  		//min parameter byte length
	uint16_t		LenMax;  		//max parameter byte length
	ParamFlags_t	Flags;  			//access flags
//	int8_t			Scaler; 		//exponent base 10
//	ParamUnit_t		Unit; 			//unit
} OBISMapping_t;

//instances
extern const OBISMapping_t OBIS_Mapping[];
extern const uint16_t OBIS_MappingCnt;

OBISMapping_t* OBIS_MappingGet(ParamID_t inPID);
OBISMapping_t* OBIS_MappingGet2(uint8_t* inOBIS);

#endif //OBIS_MAPPING_H
