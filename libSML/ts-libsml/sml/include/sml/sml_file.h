// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_FILE_H_
#define	_SML_FILE_H_

#include "sml_message.h"
#include "sml_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

// a SML file consist of multiple SML messages
typedef struct {
    sml_message **messages;
    short messages_len;
    sml_buffer *buf;
} sml_file;

sml_file *sml_file_init(int flags);
sml_file *sml_file_init2(size_t buffer_len, int flags);
sml_file *sml_file_parse(unsigned char *buffer, size_t buffer_len, int flags);
int sml_file_write(sml_file *file);
void sml_file_free(sml_file *file);

int sml_file_add_message(sml_file *file, sml_message *message);
void sml_file_print(sml_file *file);

// Returns buffer.
unsigned char *sml_file_get_buffer(sml_file *file);
int sml_file_get_buffer_len(sml_file *file);

// Adds flags.
int sml_file_add_flags(sml_file *file, int flags);

// Returns error.
int sml_file_get_error(sml_file *file);

#ifdef __cplusplus
}
#endif

#endif /* _SML_FILE_H_ */
