// Copyright 2015 Uwe Heuert

#ifndef _SML_V105_H_
#define	_SML_V105_H_

#include "sml_shared.h"
#include "sml_boolean.h"
#include "sml_octet_string.h"
#include "sml_time.h"
#include "sml_status.h"

//SML_ListType ::= CHOICE
//{
//	smlTime [0x01] SML_Time
//	smlTimestampedValue [0x02] SML_TimestampedValue
//	smlCosemValue [0x03] SML_CosemValue
//}

//SML_TimestampedValue ::= SEQUENCE
//{
//	smlTime SML_Time
//	status SML_Status
//	simpleValue SML_SimpleValue
//}

//SML_SimpleValue ::= IMPLICIT CHOICE
//{
//	boolean-Value boolean,
//	byte-List Octet String6,
//	8-Bit-Integer Integer8,
//	16-Bit-Integer Integer16,
//	32-Bit-Integer Integer32,
//	64-Bit-Integer Integer64,
//	8-Bit-Unsigned Unsigned8,
//	16-Bit-Unsigned Unsigned16,
//	32-Bit-Unsigned Unsigned32,
//	64-Bit-Unsigned Unsigned64
//}

//SML_CosemValue := CHOICE
//{
//	scaler_unit [1] SML_CosemScalerUnit
//}

//SML_CosemScalerUnit :=
//{
//	scaler Integer8
//	unit Unsigned8
//}

#ifdef __cplusplus
extern "C" {
#endif

#define SML_LIST_TYPE_TIME 0x01
#define SML_LIST_TYPE_TIMESTAMPED_VALUE 0x02
#define SML_LIST_TYPE_COSEM_VALUE 0x03

#define SML_COSEM_VALUE_SCALER_UNIT 0x01

typedef struct {
	u8 type;
	union {
		sml_boolean *boolean;
		octet_string *bytes;
		i8 *int8;
		i16 *int16;
		i32 *int32;
		i64 *int64;
		u8 *uint8;
		u16 *uint16;
		u32 *uint32;
		u64 *uint64;
	} data;
} sml_simple_value;

typedef struct {
	sml_time *time;
	sml_status *status;
	sml_simple_value *simple_value;
} sml_timestamped_value;

typedef struct {
	i8 *scaler;
	u8 *unit;
} sml_cosem_scaler_unit;

typedef struct {
	u8 *tag;
	union {
		sml_cosem_scaler_unit *scaler_unit;
	} data;
} sml_cosem_value;

typedef struct {
	u8 *tag;
	union {
		sml_time *time;
		sml_timestamped_value *timestamped_value;
		sml_cosem_value *cosem_value;
	} data;
} sml_list_type;

sml_simple_value *sml_simple_value_init();
sml_simple_value *sml_simple_value_parse(sml_buffer *buf);
void sml_simple_value_write(sml_simple_value *simple_value, sml_buffer *buf);
void sml_simple_value_free(sml_simple_value *simple_value);

sml_timestamped_value *sml_timestamped_value_init();
sml_timestamped_value *sml_timestamped_value_parse(sml_buffer *buf);
void sml_timestamped_value_write(sml_timestamped_value *timestamped_value, sml_buffer *buf);
void sml_timestamped_value_free(sml_timestamped_value *timestamped_value);

sml_cosem_scaler_unit *sml_cosem_scaler_unit_init();
sml_cosem_scaler_unit *sml_cosem_scaler_unit_parse(sml_buffer *buf);
void sml_cosem_scaler_unit_write(sml_cosem_scaler_unit *cosem_scaler_unit, sml_buffer *buf);
void sml_cosem_scaler_unit_free(sml_cosem_scaler_unit *cosem_scaler_unit);

sml_cosem_value *sml_cosem_value_init();
sml_cosem_value *sml_cosem_value_parse(sml_buffer *buf);
void sml_cosem_value_write(sml_cosem_value *cosem_value, sml_buffer *buf);
void sml_cosem_value_free(sml_cosem_value *cosem_value);

sml_list_type *sml_list_type_init();
sml_list_type *sml_list_type_parse(sml_buffer *buf);
void sml_list_type_write(sml_list_type *list_type, sml_buffer *buf);
void sml_list_type_free(sml_list_type *list_type);

#ifdef __cplusplus
}
#endif

#endif /* _SML_V105_H_ */

