#include "queue.h"

static char bnt4[] = {0, 'A', 'C', 0, 'G', 0, 0, 0, 'T', 0, 0, 0, 0, 0, 0, 'N'};

void queue_init(struct queue_t *q)
{
	q->it = 0;
	q->cnt = 32;
	__CALLOC(q->sz, q->cnt);
	__CALLOC(q->val, q->cnt);
}

void queue_destroy(struct queue_t *q)
{
	int i;
	for (i = 0; i < q->cnt; ++i)
		__FREE_AND_NULL(q->val[i]);
	__FREE_AND_NULL(q->val);
	__FREE_AND_NULL(q->sz);
}

void queue_move(struct queue_t *q)
{
	__FREE_AND_NULL(q->val[q->it]);
	++q->it;
	if (q->it == (q->cnt >> 1)) {
		memcpy(q->val, q->val + q->it, q->it * sizeof(*(q->val)));
		memcpy(q->sz, q->sz + q->it, q->it * sizeof(*(q->sz)));
		memset(q->val + q->it, 0, q->it * sizeof(*(q->val)));
		memset(q->sz + q->it, 0, q->it * sizeof(*(q->sz)));
		q->it = 0;
	}
}

static void check_bound(struct queue_t *q, int pos)
{
	while (pos >= q->cnt) {
		int old_cnt = q->cnt;
		q->cnt <<= 1;
		__REALLOC(q->val, q->cnt);
		__REALLOC(q->sz, q->cnt);
		memset(q->val + old_cnt, 0,
			old_cnt * sizeof(*(q->val)));
		memset(q->sz + old_cnt, 0,
			old_cnt * sizeof(*(q->sz)));
	}
}

void queue_push(struct queue_t *q, bam1_t *b)
{
	assert(b);
	uint32_t *cigar = bam_get_cigar(b);
	uint32_t r_pos = q->it;
	uint8_t *seq = bam_get_seq(b);
	uint8_t *qual = bam_get_qual(b);
	struct var_cand_t ret;
	int i, j, s_pos;

	for (i = s_pos = 0; i < b->core.n_cigar; ++i) {
		int oplen = bam_cigar_oplen(cigar[i]);
		char opchr = bam_cigar_opchr(cigar[i]);
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
				int *sz;
				/* skip base N */
				if (bam_seqi(seq, s_pos) == 15)
					continue;
				ret.nt4 = bnt4[bam_seqi(seq, s_pos)];
				/* base quality could not exceed mapping quality */
				ret.baseq = __min(qual[s_pos], b->core.qual);
				check_bound(q, r_pos);
				sz = q->sz + r_pos;
				__REALLOC(q->val[r_pos], *sz + 1);
				q->val[r_pos][*sz] = ret;
				++(*sz);
				r_pos++;
				s_pos++;
			}
		} else {
			__VERBOSE("%c\n", opchr);
			assert(false);
		}
	}
}
