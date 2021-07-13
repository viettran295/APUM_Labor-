#ifndef MISC_H
#define MISC_H

#include <stdbool.h>
#include <stdint.h>

bool Misc_ByteArrayCompare(void* inArray1, void* inArray2, uint16_t inLength);
//uint8_t Misc_GetRandomByte();
//char* Misc_GetSourceCode(uint8_t* data, uint16_t len);
//uint8_t Misc_OctetStringToUint8(octet_string* octet_string);
//uint16_t Misc_OctetStringToUint16(octet_string* octet_string);
//uint32_t Misc_OctetStringToUint32(octet_string* octet_string);
//uint64_t Misc_OctetStringToUint64(octet_string* octet_string);
//uint16_t Misc_SMLStringToUint16(uint8_t* str);
//uint32_t Misc_SMLStringToUint32(uint8_t* str);
//uint64_t Misc_SMLStringToUint64(uint8_t* str);
uint16_t Misc_SwapByteOrder16(uint16_t value);
uint32_t Misc_SwapByteOrder32(uint32_t value);
uint64_t Misc_SwapByteOrder64(uint64_t value);
//uint16_t Misc_ToShort(uint8_t low, uint8_t high);
//void Misc_FromShort(uint16_t number, uint8_t *low, uint8_t *high);
//uint16_t Misc_ComputeCRC16(uint8_t* data, uint16_t length);

#endif // !MISC_H
