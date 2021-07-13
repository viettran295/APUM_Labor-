// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_TRANSPORT_MEM_H_
#define	_SML_TRANSPORT_MEM_H_

#include <sml/sml_file.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char esc_seq_default[];
extern unsigned char start_seq[];
extern unsigned char end_seq[];

size_t sml_transport_read(unsigned char *mem, unsigned char *buffer, size_t max_len);
size_t sml_transport_read2(unsigned char *mem, unsigned char *buffer, size_t max_len, int flags, unsigned char *esc_seq);

int sml_transport_write(sml_file *file);
int sml_transport_write2(sml_file *file, int flags, unsigned char *esc_seq);
int sml_transport_write3(sml_buffer *buf, sml_buffer *bufMsg, int flags, unsigned char *esc_seq);

#ifdef __cplusplus
}
#endif


#endif /* _SML_TRANSPORT_MEM_H_ */
