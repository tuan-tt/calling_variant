#ifndef _VARIANT_H_
#define _VARIANT_H_

#include "utils.h"
#include "bam.h"
struct var_cand_t {
	int baseq;
	char nt4;
};

void variant_init(int n_target);

void variant_destroy(int n_target);

void variant_process(struct var_cand_t *cand_list, int cand_sz,
		     uint32_t ref_pos, char ref_nt, int id);

void variant_merge(int n_target, struct bam_inf_t *bam_inf);

#endif /* _VARIANT_H_ */
