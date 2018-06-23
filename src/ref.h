#ifndef _REF_H_
#define _REF_H_

#include "utils.h"

void ref_load(char *file_path);

void ref_destroy();

int ref_getlen(int id);

int ref_getid(char *s);

char *ref_getseq(int id);

char *ref_getname(int id);

#endif /* _REF_H_ */
