// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_CRC16_H_
#define _SML_CRC16_H_

#include "sml_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

// CRC16 FSC implementation based on DIN 62056-46
u16 sml_crc16_calculate(unsigned char *cp, int len) ;

#ifdef __cplusplus
}
#endif

#endif /* _SML_CRC16_H_ */
