// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_OCTET_STRING_H_
#define	_SML_OCTET_STRING_H_

#include <string.h>
#include "sml_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned char *str;
	int len;
} octet_string;

octet_string *sml_octet_string_init(unsigned char *str, int length);
// Parses a octet string, updates the buffer accordingly, memory must be free'd elsewhere.
octet_string *sml_octet_string_parse(sml_buffer *buf);
int sml_octet_string_write(octet_string *str, sml_buffer *buf);
void sml_octet_string_free(octet_string *str);

octet_string *sml_octet_string_init_from_hex(char *str);
octet_string *sml_octet_string_generate_uuid(void);
int sml_octet_string_cmp(octet_string *s1, octet_string *s2);
int sml_octet_string_cmp_with_hex(octet_string *str, char *hex);

// sml signature
typedef octet_string sml_signature;
#define sml_signature_parse(buf) sml_octet_string_parse(buf)
#define sml_signature_write(s, buf) sml_octet_string_write(s, buf)
#define sml_signature_free(s) sml_octet_string_free(s)

#ifdef __cplusplus
}
#endif

#endif /* _SML_OCTET_STRING_H_ */
