#include "variant.h"

void variant_process(struct var_cand_t *cand_list, int cand_sz, int ref_pos, char ref_nt)
{
	bool is_candidate = false;
	int i;

	if (ref_nt == 'N')
		return;
	
	for (i = 0; i < cand_sz; ++i) {
		if (cand_list[i].nt4 != 'N' && cand_list[i].nt4 != ref_nt) {
			is_candidate = true;
			break;
		}
	}

	if (!is_candidate)
		return;

	// __VERBOSE("ref_nt: %c, ref_pos: %u\n", ref_nt, ref_pos);
	// for (i = 0; i < cand_sz; ++i) {
	// 	__VERBOSE("\t%d %d %c\n", cand_list[i].mapq, cand_list[i].baseq, cand_list[i].nt4);
	// }
	// usleep(300);
}
