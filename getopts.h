/*******************************************************
 * wischk: the "wise" checkers playing program         *
 * Copyright (C) 1993  John J. Wiseman                 *
 * See file COPYING for license and (lack of) warranty *
 *******************************************************/

/* getopts.h */

#if !defined(__GETOPTS__)

#define GETOPTS
enum opt_type { OPTINT, OPTSTR, OPTBOOL, OPTLONG, OPTFLOAT, OPTDOUBLE };

struct opt_t {
    char *sw;
    opt_type opttyp;
    void *var;
};

int getopts(int argc, char **argv, opt_t opttable[]);

#endif

