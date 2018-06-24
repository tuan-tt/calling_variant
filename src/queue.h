#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "utils.h"
#include "variant.h"
#include "htslib/sam.h"

struct queue_t {
	struct var_cand_t **val;
	int *sz;
	int cnt;
	int it;
};

void queue_init(struct queue_t *q);

void queue_destroy(struct queue_t *q);

void queue_push(struct queue_t *q, bam1_t *b);

void queue_move(struct queue_t *q);

#endif /* _QUEUE_H_ */
