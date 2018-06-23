#include "argument.h"
#include "bam.h"
#include "ref.h"

static pthread_mutex_t lock_id = PTHREAD_MUTEX_INITIALIZER;

static void *process_pool(void *data)
{
	struct bam_inf_t *bam_inf = (struct bam_inf_t *)data;
	int id;

	do {
		pthread_mutex_lock(&lock_id);
		if (bam_inf->cur_id == bam_inf->b_hdr->n_targets) {
			pthread_mutex_unlock(&lock_id);
			pthread_exit(NULL);
			return NULL;
		} else {
			id = bam_inf->cur_id++;
		}
		pthread_mutex_unlock(&lock_id);

		read_bam_target(bam_inf, id);
	} while (1);
}

static void read_bam(struct bam_inf_t *bam_inf)
{
	__VERBOSE("Calculating ... \n");

	int i;

	/* get bam stats by parallel */
	pthread_t *pthr = calloc(args.n_thread, sizeof(pthread_t));;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < args.n_thread; ++i)
		pthread_create(&pthr[i], &attr, process_pool, bam_inf);
	for (i = 0; i < args.n_thread; ++i)
		pthread_join(pthr[i], NULL);

	free(pthr);
	pthread_attr_destroy(&attr);
}

int main(int argc, char *argv[])
{
	struct bam_inf_t bam_inf;

	get_args(argc, argv);
	ref_load(args.reference);	
	bam_inf_init(&bam_inf, args.in_bam);
	read_bam(&bam_inf);

	return 0;
}
