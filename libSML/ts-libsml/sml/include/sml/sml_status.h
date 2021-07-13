// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_STATUS_H_
#define	_SML_STATUS_H_

#include "sml_number.h"
#include "sml_shared.h"

//SML_Status ::= IMPLICIT CHOICE
//{
//	status8 Unsigned8
//	status16 Unsigned16
//	status32 Unsigned32
//	status64 Unsigned64
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	u8 type;
	union {
		u8 *status8;
		u16 *status16;
		u32 *status32;
		u64 *status64;
	} data;
} sml_status;

sml_status *sml_status_init(void);
sml_status *sml_status_parse(sml_buffer *buf);
int sml_status_write(sml_status *status, sml_buffer *buf);
void sml_status_free(sml_status *status);

#ifdef __cplusplus
}
#endif

#endif /* _SML_STATUS_H_ */
