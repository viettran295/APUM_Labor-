// Copyright 2015 - 2019
// Uwe Heuert
// exceeding solutions GmbH
//
// This file is part of libSML.

#ifndef _SML_SIGNATURE_H_
#define	_SML_SIGNATURE_H_

#include <stdarg.h>
#include <sml/sml_get_proc_parameter_response.h>

#ifdef __cplusplus
extern "C" {
#endif

octet_string *sml_list_entry_build_signed_content(int type, octet_string *server_id, sml_list *list, octet_string *log_book);
octet_string *sml_tree_build_signed_content(int type, octet_string *server_id, sml_tree *tree);

#ifdef __cplusplus
}
#endif

#endif /* _SML_SIGNATURE_H_ */
