#include "bam.h"
#include "ref.h"
#include "queue.h"
#include "variant.h"

static char bnt4[] = {0, 'A', 'C', 0, 'G', 0, 0, 0, 'T', 0, 0, 0, 0, 0, 0, 'N'};

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

struct var_cand_t check_contain(bam1_t *b, uint32_t tag_pos)
{
	uint32_t *cigar = bam_get_cigar(b);
	uint32_t r_pos = b->core.pos;
	uint8_t *seq = bam_get_seq(b);
	uint8_t *qual = bam_get_qual(b);
	struct var_cand_t ret;
	int i, j, s_pos;

	for (i = s_pos = 0; i < b->core.n_cigar; ++i) {
		int oplen = bam_cigar_oplen(cigar[i]);
		char opchr = bam_cigar_opchr(cigar[i]);
		if (r_pos > tag_pos) {
			ret.nt4 = '\0';
			return ret;
		}
		if (opchr == 'N') {
			r_pos += oplen;
		} else if (opchr == 'D') {
			r_pos += oplen;
		} else if (opchr == 'I') {
			s_pos += oplen;
		} else if (opchr == 'S') {
			s_pos += oplen;
		} else if (opchr == 'M') {
			for (j = 0; j < oplen; ++j) { 
				if (r_pos == tag_pos) {
					ret.nt4 = bnt4[bam_seqi(seq, s_pos)];
					ret.mapq = b->core.qual;
					ret.baseq = qual[s_pos];
					return ret;	
				}
				r_pos++;
				s_pos++;
			}
		} else {
			__VERBOSE("%c\n", opchr);
			assert(false);
		}
	}

	ret.nt4 = '\0';
	return ret;
}

void read_bam_target(struct bam_inf_t *bam_inf, int id)
{
	__VERBOSE("%s\n", bam_inf->b_hdr->target_name[id]);
	int ref_id = ref_getid(bam_inf->b_hdr->target_name[id]);
	uint32_t target_len = bam_inf->b_hdr->target_len[id];
	char *ref_seq = ref_getseq(ref_id);
	assert(ref_getlen(ref_id) == target_len);
	samFile *in_bam_f = sam_open(bam_inf->bam_path, "rb");
	hts_itr_t *iter = sam_itr_queryi(bam_inf->bam_i, id, 1, target_len);
	bam1_t *b = bam_init1();
	struct queue_t queue;
	queue_init(&queue);
	uint32_t tag_pos;
	int i;

	b = get_nxt_bam1(in_bam_f, iter);
	if (b == NULL)
		return;

	for (tag_pos = 0; tag_pos < target_len; ++tag_pos) {
		struct var_cand_t temp;
		struct var_cand_t *cand_list;
		__CALLOC(cand_list, 32);
		int cand_sz = 0, alloc_sz = 32;

		while (b && b->core.pos <= tag_pos) {
			queue_push(&queue, b);
			b = get_nxt_bam1(in_bam_f, iter);
			if (b == NULL)
				break;
		}
		for (i = queue.head; i <= queue.tail; ++i) {
			if (i >= queue.sz)
				i = 0;
			temp = check_contain(queue.val[i], tag_pos);
			if (temp.nt4 == '\0')
				continue;
			if (cand_sz == alloc_sz) {
				alloc_sz <<= 1;
				__REALLOC(cand_list, alloc_sz);
				memset(cand_list, 0, (alloc_sz >> 1)
					* sizeof(struct var_cand_t));
			}
			cand_list[cand_sz++] = temp;
		}

		if (cand_sz > 0) {
			/* calling variant process */
		}
	}

	sam_itr_destroy(iter);
	bam_destroy1(b);
	sam_close(in_bam_f);
}
