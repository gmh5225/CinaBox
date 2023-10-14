#ifndef __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_H__
#define __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_H__

#include "libtcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Unpack the libraries storied in the binary and configure them automatically, with options to leave out the packed standard library if a custom one is desired*/
LIBTCCAPI void tcc_setup_c_in_a_box(TCCState *s);
LIBTCCAPI void tcc_setup_c_in_a_box_no_includes(TCCState *s);
LIBTCCAPI void tcc_setup_c_in_a_box_no_libc(TCCState *s);

/* Compile the provided C source code, if there is a hashbang in the first line any command line options following it are applied */
LIBTCCAPI int tcc_compile_hashbang_string(TCCState *s, const char *str);

/* Add a C file, if there is a hashbang in the first line any command line options following it are applied */
LIBTCCAPI int tcc_add_hashbang_file(TCCState *s, const char *filepath);

#ifdef __cplusplus
}
#endif

#endif // __95e6ad99ac85a8011d8841ff70c3ed96_C_IN_A_BOX_H__