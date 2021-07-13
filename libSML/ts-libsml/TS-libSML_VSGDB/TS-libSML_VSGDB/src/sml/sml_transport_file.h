// Copyright 2015 Uwe Heuert

#ifndef _SML_TRANSPORT_FILE_H_
#define	_SML_TRANSPORT_FILE_H_

#include <sml/sml_file.h>

#ifdef __cplusplus
extern "C" {
#endif

// sml_transport_read reads continously bytes from fd and scans
// for the SML transport protocol escape sequences. If a SML file
// is detected it will be copied into the buffer. The total amount of bytes read
// will be returned. If the SML file exceeds the len of the buffer, -1 will be returned
size_t sml_transport_read_file(FILE *fp, unsigned char *buffer, size_t max_len);
size_t sml_transport_read_mem(unsigned char *mem, unsigned char *buffer, size_t max_len);

// sml_transport_writes adds the SML transport protocol escape
// sequences and writes the given file to fd. The file must be
// in the parsed format.
// The number of bytes written is returned, 0 if there was an
// error.
// The sml_file must be free'd elsewhere.
int sml_transport_write_file(FILE *fp, sml_file *file);

#ifdef __cplusplus
}
#endif


#endif /* _SML_TRANSPORT_FILE_H_ */

