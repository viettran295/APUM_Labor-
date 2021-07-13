// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_LOG_H_
#define	_SML_LOG_H_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SML_TRACE_NONE		0
#define SML_TRACE_PRINTF	1
#define SML_TRACE_CB		2

typedef int (*TRACE_CB)(const char *format, va_list arguments);

void sml_set_trace_cb(int trace_t, TRACE_CB function_trace_cb);

void trace(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _SML_LOG_H_ */
