#include "bam.h"

void bam_inf_init(struct bam_inf_t *bam_inf, const char *file_path)
{
	bam_inf->bam_path = file_path;
	bam_inf->cur_id = 0;

	/* Open bam file and the index file */
	samFile *bam_f;
	if (!(bam_f = sam_open(file_path, "rb")))
		__PERROR("Could not open BAM file");
	if (!(bam_inf->bam_i = sam_index_load(bam_f, file_path)))
		__ERROR("BAM file must be indexed!");

	/* Init the header */
	bam_inf->b_hdr = sam_hdr_read(bam_f);
	sam_close(bam_f);
}

void read_bam_target(struct bam_inf_t *bam_inf, int id)
{
	int target_len = bam_inf->b_hdr->target_len[id];
	samFile *in_bam_f = sam_open(bam_inf->bam_path, "rb");
	hts_itr_t *iter = sam_itr_queryi(bam_inf->bam_i, id, 1, target_len);
	bam1_t *b = bam_init1();
	
	while (sam_itr_next(in_bam_f, iter, b) >= 0) {
		/* skip align is supplementary or not primary */
		if (b->core.flag & (FLAG_NOT_PRI | FLAG_SUPPLEMENT))
			continue;

		/* skip unmapped read */
		if (b->core.n_cigar == 0)
			continue;	
	}

	sam_itr_destroy(iter);
	bam_destroy1(b);
	sam_close(in_bam_f);
}
