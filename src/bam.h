#ifndef _BAM_H_
#define _BAM_H_

#include "utils.h"
#include "htslib/sam.h"

/* Sam flag */
#define FLAG_PAIR		0x001	/* Read has its mate */
#define FLAG_PROPER 		0x002	/* Read and its mate are proper pair */
#define FLAG_UNMAP 		0x004	/* Read is unmapped */
#define FLAG_M_UNMAP		0x008	/* Read's mate is unmapped */
#define FLAG_REVERSE 		0x010	/* Read is reversed */
#define FLAG_M_REVERSE 		0x020	/* Read's mate is reversed */
#define FLAG_READ1 		0x040	/* Read is first read */
#define FLAG_READ2 		0x080	/* Read is second read */
#define FLAG_NOT_PRI		0x100	/* Alginment is not primary */
#define FLAG_FAIL		0x200	/* Read fails platform/vendor quality checks */
#define FLAG_DUPLICATE 		0x400	/* Read is PCR or optical duplicate */
#define FLAG_SUPPLEMENT		0x800	/* Alignment is supplementary */

struct bam_inf_t {
	const char *bam_path;
	hts_idx_t *bam_i;
	bam_hdr_t *b_hdr;
	int cur_id;
};

void bam_inf_init(struct bam_inf_t *bam_inf, const char *file_path);

void read_bam_target(struct bam_inf_t *bam_inf, int id);

#endif /* _BAM_H_ */
