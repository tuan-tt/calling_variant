#ifndef _REF_H_
#define _REF_H_

#include "utils.h"

char **ref_load(char *file_path);

void ref_destroy(char **ref);

#endif /* _REF_H_ */
