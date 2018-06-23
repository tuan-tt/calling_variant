#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "utils.h"
#include "htslib/sam.h"

struct queue_t {
	bam1_t **val;
	int sz;
	int head;
	int tail;
};

void queue_init(struct queue_t *q);

void queue_destroy(struct queue_t *q);

void queue_push(struct queue_t *q, bam1_t *b);

bam1_t *queue_pop(struct queue_t *q);

#endif /* _QUEUE_H_ */
