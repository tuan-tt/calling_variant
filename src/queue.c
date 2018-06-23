#include "queue.h"

void queue_init(struct queue_t *q)
{
	q->tail = 0;
	q->head = 1;
	q->sz = 32;
	__CALLOC(q->val, q->sz);
}

void queue_destroy(struct queue_t *q)
{
	free(q->val);
}

void queue_push(struct queue_t *q, bam1_t *b)
{
	q->tail++;
	if (q->tail == q->head) {
		int old_sz, num;
		old_sz = q->sz;
		num = q->sz - q->tail - 1;
		q->sz <<= 1;
		__REALLOC(q->val, q->sz);
		memset(q->val + old_sz, 0, old_sz * sizeof(bam1_t *));
		memcpy(q->val + q->tail, q->val + q->sz - num, num * sizeof(bam1_t *));
	}
	q->val[q->tail] = bam_dup1(b);
}

bam1_t *queue_pop(struct queue_t *q)
{
	assert(q->val[q->head]);
	q->head++;
	return q->val[q->head - 1];
}
