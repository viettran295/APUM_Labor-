// Copyright 2015 Uwe Heuert

#ifndef _SML_SIGNATURE_H_
#define	_SML_SIGNATURE_H_

#include <stdarg.h>
#include <sml/sml_get_proc_parameter_response.h>

#ifdef __cplusplus
extern "C" {
#endif

void sml_get_proc_parameter_response_build_signed_content(int type, sml_get_proc_parameter_response *smlGetProcParameterResponse, unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _SML_SIGNATURE_H_ */

