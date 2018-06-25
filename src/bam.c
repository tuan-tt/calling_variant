#include "bam.h"
#include "ref.h"
#include "queue.h"
#include "variant.h"

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

static bam1_t *get_nxt_bam1(samFile *in_bam_f, hts_itr_t *iter)
{
	bam1_t *b = bam_init1();
	while (1) {
		if (sam_itr_next(in_bam_f, iter, b) < 0)
			return NULL;
		/* skip unneed read */
		if (b->core.flag &
		    (FLAG_NOT_PRI | FLAG_SUPPLEMENT | FLAG_DUPLICATE | FLAG_FAIL)) {
			continue;
		}
		/* skip unmapped read */
		if (b->core.n_cigar == 0)
			continue;
		return b;
	}
}

void read_bam_target(struct bam_inf_t *bam_inf, int id)
{
	/* get target ref info */
	int ref_id = ref_getid(bam_inf->b_hdr->target_name[id]);
	uint32_t target_len = bam_inf->b_hdr->target_len[id];
	char *ref_seq = ref_getseq(ref_id);
	assert(ref_getlen(ref_id) == target_len);

	/* init bam query */
	samFile *in_bam_f = sam_open(bam_inf->bam_path, "rb");
	hts_itr_t *iter = sam_itr_queryi(bam_inf->bam_i, id, 1, target_len);
	bam1_t *b;
	uint32_t tag_pos;

	/* init queue & cand list */
	struct queue_t queue;
	queue_init(&queue);

	b = get_nxt_bam1(in_bam_f, iter);
	if (b == NULL)
		return;

	for (tag_pos = 0; tag_pos < target_len; ++tag_pos) {
		/* get new bam */
		while (b && b->core.pos == tag_pos) {
			queue_push(&queue, b);
			bam_destroy1(b);
			b = get_nxt_bam1(in_bam_f, iter);
			if (b == NULL)
				break;
		}

		/* get candidate list and process */
		if (queue.sz[queue.it] > 0)
			variant_process(queue.val[queue.it], queue.sz[queue.it],
					tag_pos, nt4_char[nt4_table[ref_seq[tag_pos]]], id);

		queue_move(&queue);
	}

	assert(b == NULL);
	queue_destroy(&queue);
	sam_itr_destroy(iter);
	bam_destroy1(b);
	sam_close(in_bam_f);
}
