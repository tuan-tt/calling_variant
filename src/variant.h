#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "utils.h"

struct var_cand_t {
	int mapq;
	int baseq;
	char nt4;
};

void variant_process(struct var_cand_t *cand_list, int cand_sz, int ref_pos, char ref_nt);

#endif /* _VARIANT_H_ */
