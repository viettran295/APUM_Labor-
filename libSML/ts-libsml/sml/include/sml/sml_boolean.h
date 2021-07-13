// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_BOOLEAN_H_
#define	_SML_BOOLEAN_H_

//#define SML_BOOLEAN_TRUE 0xFF
#define SML_BOOLEAN_TRUE 0x01
#define SML_BOOLEAN_FALSE 0x00

#include "sml_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u8 sml_boolean;

sml_boolean *sml_boolean_init(u8 b);
sml_boolean *sml_boolean_parse(sml_buffer *buf);
int sml_boolean_write(sml_boolean *boolean, sml_buffer *buf);
void sml_boolean_free(sml_boolean *b);

#ifdef __cplusplus
}
#endif

#endif /* _SML_BOOLEAN_H_ */
