#ifndef HDLC_ERROR_MSG_H
#define HDLC_ERROR_MSG_H

#include <stdint.h>

/*
 *	enum values:
 *	
 *	0 == OK
 *	[0xfd00, 0xff00] ATTN @ SML_Protocol.h
 *	[0, 10] HDLC_Factory.h
 *	[-400, 0] Serial.h
 *	[-1020, -1000] HDLC_HL_Client.h
 *	[-2010, -2000] HDLC_Meter.h
 *	[-3010, -3000] SML_Protocol.h
 *	[-4010, -4000] SML_Factory.h
 *
 **/

char* LMN_GetErrorMessage(int32_t inValue);

#endif // !HDLC_ERROR_MSG_H
