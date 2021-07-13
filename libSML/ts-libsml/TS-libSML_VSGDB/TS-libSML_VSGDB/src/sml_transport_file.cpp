// Copyright 2015 Uwe Heuert

#include <stdio.h>
#include <time.h>
#include <string.h>

#ifndef USE_HAL_DRIVER
	#include <sys/types.h>
	#ifndef WIN32
		#include <unistd.h>
	#endif
#endif

#include <sml/sml_transport.h>
#include <sml/sml_transport_file.h>
#include <sml/sml_shared.h>
#include <sml/sml_crc16.h>
#include <sml/sml_log.h>

#define MC_SML_BUFFER_LEN 8096

extern unsigned char esc_seq[];
extern unsigned char start_seq[];
extern unsigned char end_seq[];


size_t sml_read_file(FILE *fp, unsigned char *buffer, size_t len) {
	
	int r;
	size_t tr = 0;

	while (tr < len) {
		r = fread(&(buffer[tr]), 1, len - tr, fp);
		if (r < 0) continue;

		tr += r;
	}
	return tr;
}

size_t sml_transport_read_file(FILE *fp, unsigned char *buffer, size_t max_len) {
	//unsigned char buf[max_len];
	size_t rv = 0;
	unsigned char *buf = (unsigned char *)malloc(max_len);
	if (!buf) {
		trace("ERROR: malloc %ld bytes\n", max_len);
		return 0;
	}

	memset(buf, 0, max_len);
	unsigned int len = 0;
	
	while (len < 8) {
		sml_read_file(fp, &(buf[len]), 1);

		if ((buf[len] == 0x1b && len < 4) || (buf[len] == 0x01 && len >= 4)) {
			len++;
		}
		else {
			len = 0;
		}
	}

	// found start sequence

	while (len < max_len) {
		
		sml_read_file(fp, &(buf[len]), 4);
			
		if (memcmp(&buf[len], esc_seq, 4) == 0) {
			
			// found esc sequence
			len += 4;
			sml_read_file(fp, &(buf[len]), 4);
			
			if (buf[len] == 0x1a) {

				// found end sequence
				len += 4;

				u16 crc = sml_crc16_calculate(&(buf[0]), len - 2);
				unsigned char crcBuf[2];
				crcBuf[0] = (unsigned char) ((crc & 0xFF00) >> 8);
				crcBuf[1] = (unsigned char) (crc & 0x00FF);
				if (memcmp(crcBuf, &(buf[len - 2]), 2) == 0) {
					//memcpy(buffer, &(buf[0]), len);
					len -= 16;
					memcpy(buffer, &(buf[8]), len);
					rv = len;
					goto end;
				}
				else {
					goto end;
				}
			}
			else {
				// dont read other escaped sequences yet
				trace("error: unrecognized sequence\n");
				goto end;
			}
		}
		len += 4;

	}

end:
	if (buf)
		free(buf);

	return rv;
}

size_t sml_transport_read_mem(unsigned char *mem, unsigned char *buffer, size_t max_len) {
	size_t rv = 0;
	unsigned char *buf = (unsigned char *)malloc(max_len);
	if (!buf) {
		trace("ERROR: malloc %ld bytes\n", max_len);
		return 0;
	}

	memset(buf, 0, max_len);
	unsigned int len = 0;
	
	while (len < 8) {
		memcpy(&buf[len], &mem[len], 1);

		if ((buf[len] == 0x1b && len < 4) || (buf[len] == 0x01 && len >= 4)) {
			len++;
		}
		else {
			len = 0;
		}
	}

	// found start sequence

	while (len < max_len) {
		
		memcpy(&buf[len], &mem[len], 4);

		if (memcmp(&buf[len], esc_seq, 4) == 0) {
			
			// found esc sequence
			len += 4;
			memcpy(&buf[len], &mem[len], 4);
			
			if (buf[len] == 0x1a) {

				// found end sequence
				len += 4;

				u16 crc = sml_crc16_calculate(&(buf[0]), len - 2);
				unsigned char crcBuf[2];
				crcBuf[0] = (unsigned char) ((crc & 0xFF00) >> 8);
				crcBuf[1] = (unsigned char) (crc & 0x00FF);
				if (memcmp(crcBuf, &(buf[len - 2]), 2) == 0) {
					//memcpy(buffer, &(buf[0]), len);
					len -= 16;
					memcpy(buffer, &(buf[8]), len);
					rv = len;
					goto end;
				}
				else {
					goto end;
				}
			}
			else {
				// dont read other escaped sequences yet
				trace("error: unrecognized sequence\n");
				goto end;
			}
		}
		len += 4;

	}

end:
	if (buf)
		free(buf);

	return rv;
}

int sml_transport_write_file(FILE *fp, sml_file *file) {
	sml_buffer *buf = file->buf;
	buf->cursor = 0;

	memset(buf->buffer, 0, buf->buffer_len);

	// add start sequence
	memcpy(sml_buf_get_current_buf(buf), start_seq, 8);
	buf->cursor += 8;

	// add file
	sml_file_write(file);

	// add padding bytes
	int padding = (buf->cursor % 4) ? (4 - buf->cursor % 4) : 0;
	if (padding) {
		// write zeroed bytes
		memset(sml_buf_get_current_buf(buf), 0, padding);
		buf->cursor += padding;
	}

	// begin end sequence
	memcpy(sml_buf_get_current_buf(buf), end_seq, 5);
	buf->cursor += 5;

	// add padding info
	buf->buffer[buf->cursor++] = (unsigned char) padding;

	// add crc checksum
	u16 crc = sml_crc16_calculate(buf->buffer, buf->cursor);
	buf->buffer[buf->cursor++] = (unsigned char) ((crc & 0xFF00) >> 8);
	buf->buffer[buf->cursor++] = (unsigned char) (crc & 0x00FF);

	size_t wr;
	if (fp) {
		wr = fwrite(buf->buffer, 1, buf->cursor, fp);
	}
	else {
		wr = buf->cursor;
	}
	if (wr == buf->cursor) {
		return wr;
	}

	return 0;
}
