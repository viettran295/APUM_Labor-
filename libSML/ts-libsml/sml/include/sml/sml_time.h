// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_TIME_H_
#define	_SML_TIME_H_

#include "sml_shared.h"
#include "sml_number.h"

//SML_Time ::= CHOICE
//{
//	secIndex [0x01] Unsigned32,
//	timestamp [0x02] SML_Timestamp,
//	localTimestamp [0x03] SML_TimestampLocal
//}

//SML_Timestamp ::= Unsigned32

//SML_TimestampLocal ::= SEQUENCE
//{
//	timestamp SML_Timestamp,
//	localOffset Integer16,
//	seasonTimeOffset Integer16
//}

#ifdef __cplusplus
extern "C" {
#endif

#define SML_TIME_SEC_INDEX 0x01
#define SML_TIME_TIMESTAMP 0x02
#define SML_TIME_TIMESTAMP_LOCAL 0x03		//v1.05

typedef struct {
	u32 *timestamp;
	i16 *local_offset;
	i16 *season_time_offset;
} sml_timestamp_local;

typedef struct {
	u8 *tag;
	union {
		u32 *sec_index;
		u32 *timestamp;
		sml_timestamp_local *timestamp_local;				//v1.05
	}
	data;
} sml_time;

sml_timestamp_local *sml_timestamp_local_init(void);
sml_timestamp_local *sml_timestamp_local_parse(sml_buffer *buf);
int sml_timestamp_local_write(sml_timestamp_local *timestamp_local, sml_buffer *buf);
void sml_timestamp_local_free(sml_timestamp_local *timestamp_local);

sml_time *sml_time_init(void);
sml_time *sml_time_parse(sml_buffer *buf);
int sml_time_write(sml_time *time, sml_buffer *buf);
void sml_time_free(sml_time *time);
#ifdef __cplusplus
}
#endif

#endif /* _SML_TIME_H_ */
