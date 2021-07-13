// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_LIST_H_
#define	_SML_LIST_H_

#include "sml_time.h"
#include "sml_octet_string.h"
#include "sml_number.h"
#include "sml_status.h"
#include "sml_value.h"

//SML_List ::= SEQUENCE OF
//{
//	valListEntry SML_ListEntry
//}

//SML_ListEntry ::= SEQUENCE
//{
//	objName Octet String,
//	status SML_Status OPTIONAL,
//	valTime SML_Time OPTIONAL,
//	unit SML_Unit OPTIONAL,
//	scaler Integer8 OPTIONAL,
//	value SML_Value,
//	valueSignature SML_Signature OPTIONAL
//}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void **elems;
	int elems_len;
	void (*elem_free) (void *elem);
} sml_sequence;

sml_sequence *sml_sequence_init(void (*elem_free) (void *elem));
sml_sequence *sml_sequence_parse(sml_buffer *buf, void *(*elem_parse) (sml_buffer *buf), void (*elem_free) (void *elem));
int sml_sequence_write(sml_sequence *seq, sml_buffer *buf, int (*elem_write) (void *elem, sml_buffer *buf));
void sml_sequence_free(sml_sequence *seq);

int sml_sequence_add(sml_sequence *list, void *new_entry);

typedef struct sml_list_entry {
	octet_string *obj_name;
	sml_status *status; // optional
	sml_time *val_time; // optional
	sml_unit *unit; // optional
	i8 *scaler; // optional
	sml_value *value;
	sml_signature *value_signature; // optional

	// list specific
	struct sml_list_entry *next;
} sml_list;

sml_list *sml_list_init(void);
sml_list *sml_list_parse(sml_buffer *buf);
int sml_list_write(sml_list *list, sml_buffer *buf);
void sml_list_free(sml_list *list);

void sml_list_add(sml_list *list, sml_list *new_entry);

// sml_list_entry;
//sml_list *sml_list_entry_init();
sml_list *sml_list_entry_parse(sml_buffer *buf);
int sml_list_entry_write(sml_list *list, sml_buffer *buf);
void sml_list_entry_free(sml_list *list);

#ifdef __cplusplus
}
#endif

#endif /* _SML_LIST_H_ */
