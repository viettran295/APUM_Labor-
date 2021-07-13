#include "Misc.h"
#include <string.h>
#include <stdio.h>
#include <TestP.h>
#include  <stdlib.h>
#include <stm32l4xx.h>

//----------------------------------------------------------------
//helper - swap short byte order for 16 bit value
uint16_t Misc_SwapByteOrder16(uint16_t value)
{
	uint16_t tempRet = value >> 8;
	tempRet |= value << 8;
	return tempRet;
}

//----------------------------------------------------------------
//helper - swap short byte order for 32 bit value
uint32_t Misc_SwapByteOrder32(uint32_t value)
{
	uint32_t tempRet = 0;
	uint8_t* tempVp = (uint8_t*)&value;
	
	for (uint8_t i = 0; i < 4; i++)
	{
		tempRet <<= 8;
		tempRet |= tempVp[i];
	}
	
	return tempRet;
}

//----------------------------------------------------------------
//helper - swap short byte order for 64 bit value
uint64_t Misc_SwapByteOrder64(uint64_t value)
{
	uint64_t tempRet = 0;
	uint8_t* tempVp = (uint8_t*)&value;
	
	for (uint8_t i = 0; i < 8; i++)
	{
		tempRet <<= 8;
		tempRet |= tempVp[i];
	}
	
	return tempRet;
}

////----------------------------------------------------------------
////get short from bytes
//uint16_t Misc_ToShort(uint8_t low, uint8_t high)
//{
//	return ((((uint16_t)high) << 8) + low);
//}
//
////----------------------------------------------------------------
////get bytes from short
//void Misc_FromShort(uint16_t number, uint8_t *low, uint8_t *high)
//{
//	*high = (uint8_t)(number >> 8);
//	*low = (uint8_t)(number & 0xFF);
//}
//
////----------------------------------------------------------------
////helper - soft crc
//uint16_t Misc_ComputeCRC16(uint8_t* data, uint16_t length) 
//{
//	uint8_t x;
//	uint16_t crc = 0xFFFF;
//
//	while (length--) 
//	{
//		x = crc >> 8 ^ *data++;
//		x ^= x >> 4;
//		crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
//	}
//	return crc;
//}

//----------------------------------------------------------------
//compare byte arrays
bool Misc_ByteArrayCompare(void* inArray1, void* inArray2, uint16_t inLength)
{
	if (!inArray1 && !inArray2) return true;
	else if (!inArray1 || !inArray2)return false;
	else if (inLength == 0) return true;
	else
		return memcmp(inArray1, inArray2, inLength) == 0;
}

////----------------------------------------------------------------
////compare byte arrays
//uint8_t Misc_GetRandomByte()
//{
//	static bool tempOnce = true;
//	
//	if (tempOnce) 
//	{
//		srand(SysTick->VAL);
//		tempOnce = false;
//	}
//	
//	return (uint8_t) rand();
//}
//
////----------------------------------------------------------------
////create source code string form data array
//char* Misc_GetSourceCode(uint8_t* data, uint16_t len)
//{
//	static char tempRet[1024] = { 0 };
//	uint16_t iter = 0;
//	MEMSET(tempRet, 0, sizeof(tempRet));
//	
//	if ((13 + 5*len) > sizeof(tempRet))
//	{
//		sprintf(&tempRet[iter], "data too long\r\n");
//		return tempRet;
//	}
//	
//	sprintf(&tempRet[iter], "data: \r\n{");
//	iter += 9;
//	
//	for (uint16_t i = 0; i < len; i++)
//	{			
//		sprintf(&tempRet[iter], "0x%02x,", data[i]);
//		iter += 5;
//	}
//	
//	sprintf(&tempRet[iter-1], " }\r\n");
//	
//	return tempRet;
//} 
//
////----------------------------------------------------------------
////octet string convert
//uint8_t Misc_OctetStringToUint8(octet_string* octet_string)
//{
//	if (!(TEST_P(octet_string, str)) || octet_string->len < 1) return 0;
//	
//	return octet_string->str[octet_string->len - 1];
//}
//
////----------------------------------------------------------------
////octet string convert
//uint16_t Misc_OctetStringToUint16(octet_string* octet_string)
//{
//	if (!(TEST_P(octet_string, str)) || octet_string->len < 1) return 0;
//	uint16_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < octet_string->len && i < 2; i++)
//	{
//		tempRet += octet_string->str[octet_string->len -1 - i] << (8*i);
//	}
//	
//	return tempRet;
//}
//
////----------------------------------------------------------------
////octet string convert
//uint32_t Misc_OctetStringToUint32(octet_string* octet_string)
//{
//	if (!(TEST_P(octet_string, str)) || octet_string->len < 1) return 0;
//	uint32_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < octet_string->len && i < 4; i++)
//	{
//		tempRet += octet_string->str[octet_string->len - 1 - i] << (8*i);
//	}
//	
//	return tempRet;
//}
//
////----------------------------------------------------------------
////octet string convert
//uint64_t Misc_OctetStringToUint64(octet_string* octet_string)
//{
//	if (!(TEST_P(octet_string, str)) || octet_string->len < 1) return 0;
//	uint64_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < octet_string->len && i < 8; i++)
//	{
//		tempRet += octet_string->str[octet_string->len - 1 - i] << (8*i);
//	}
//	
//	return tempRet;
//}
//
////----------------------------------------------------------------
////string convert
//uint16_t Misc_SMLStringToUint16(uint8_t* str)
//{
//	if (!str) return 0;
//	
//	uint16_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < 2; i++)
//	{
//		tempRet += str[1 - i] << (8*i);
//	}
//	
//	return tempRet;
//}
//
////----------------------------------------------------------------
////string convert
//uint32_t Misc_SMLStringToUint32(uint8_t* str)
//{
//	if (!str) return 0;
//	
//	uint16_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < 4; i++)
//	{
//		tempRet += str[3 - i] << (8*i);
//	}
//	
//	return tempRet;
//}
//
////----------------------------------------------------------------
////string convert
//uint64_t Misc_SMLStringToUint64(uint8_t* str)
//{
//	if (!str) return 0;
//	
//	uint16_t tempRet = 0;
//	
//	for (uint8_t i = 0; i < 8; i++)
//	{
//		tempRet += str[7 - i] << (8*i);
//	}
//	
//	return tempRet;
//}