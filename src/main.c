#include "argument.h"
#include "bam.h"
#include "ref.h"

static pthread_mutex_t lock_id = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_verbose = PTHREAD_MUTEX_INITIALIZER;

static double real_time, cpu_time;
static int done_cnt;

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

		pthread_mutex_lock(&lock_verbose);
		++done_cnt;
		__VERBOSE("\rDone %d targets!", done_cnt);
		pthread_mutex_unlock(&lock_verbose);
	} while (1);
}

static void read_bam(struct bam_inf_t *bam_inf)
{
	__VERBOSE("Processing ... \n");

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

	__VERBOSE("\n");
	free(pthr);
	pthread_attr_destroy(&attr);
}

void show_general_info(int argc, char *argv[])
{
	int i;

	__VERBOSE("\n");
	__VERBOSE("Version: %s\n", VERSION);
	__VERBOSE("CMD: ");

	for (i = 0; i < argc; ++i) {
		__VERBOSE("%s", argv[i]);
		if (i == argc - 1)
			__VERBOSE("\n");
		else
			__VERBOSE(" ");
	}

	__VERBOSE("Real time: %.1f sec; CPU: %.1f sec\n",
		realtime() - real_time, cputime() - cpu_time);
}

int main(int argc, char *argv[])
{
	struct bam_inf_t bam_inf;
	real_time = realtime();
	cpu_time = cputime();

	get_args(argc, argv);
	ref_load(args.reference);	
	bam_inf_init(&bam_inf, args.in_bam);
	read_bam(&bam_inf);
	show_general_info(argc, argv);

	return 0;
}
