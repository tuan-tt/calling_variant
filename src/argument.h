#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_

#include "utils.h"

#define VERSION			"1.0.0"

struct prog_args {
	int n_thread;
	char *in_bam;
	char *reference;
	char *out_dir;
	char *prefix;
};

void print_usage();

void get_args(int argc, char *argv[]);

extern struct prog_args args;

#endif /* _ARGUMENT_H_ */
