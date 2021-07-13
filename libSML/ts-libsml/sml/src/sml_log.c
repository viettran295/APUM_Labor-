// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#include <stdio.h>
#include <sml/sml_log.h>

TRACE_CB	trace_cb	= NULL;
int			trace_type	= SML_TRACE_NONE;

void sml_set_trace_cb(int trace_t, TRACE_CB function_trace_cb)
{
	trace_type	= trace_t;
	trace_cb	= function_trace_cb;
}

void trace(const char *format, ...)
{
	va_list arguments;

	if (!format)
		return;

	/* Initializing arguments to store all values after format */
	va_start(arguments, format);

	if ((trace_type == SML_TRACE_CB) && trace_cb)
	{
		trace_cb(format, arguments);
	}
	else if (trace_type == SML_TRACE_PRINTF)
	{
		vfprintf(stderr, format, arguments);
	}

	// Cleans up the list
	va_end(arguments);
}
