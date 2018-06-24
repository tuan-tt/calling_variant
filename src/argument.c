#include "argument.h"

struct prog_args args;

void print_usage()
{
	__VERBOSE("./cvar <option> reference.fa input.bam\n");
	__VERBOSE("\n");
	__VERBOSE("Version: %s\n", VERSION);
	__VERBOSE("\n");
	__VERBOSE("The Bam file must be sorted by coordinate and be indexed.\n");
	__VERBOSE("Recommend using BWA to align reads to the referenece.\n");
	__VERBOSE("\n");
	__VERBOSE("Option:\n");
	__VERBOSE("  -t INT                number of threads (default: 4)\n");
	__VERBOSE("  -o DIR                output directory (default: './cvar_out/')\n");
	__VERBOSE("  -p STR                prefix of output (default: 'output')\n");
	__VERBOSE("  -h                    print help usage and exit\n");
	__VERBOSE("\n");
	__VERBOSE("This tool will generate some files in output directory:\n");
	__VERBOSE("  ${prefix}.vcf         variant calling result\n");
	__VERBOSE("  ${prefix}.log         log file\n");
	__VERBOSE("\n");
}

void get_args(int argc, char *argv[])
{
	int c, i, n;
	args.out_dir = "cvar_out";
	args.prefix = "output";
	args.n_thread = 1;

	if (argc < 3) {
		print_usage();
		exit(0);
	}

	while ((c = getopt(argc, argv, "p:t:o:")) >= 0) {
		switch (c) {
		case 't':
			args.n_thread = atoi(optarg);
			break;
		case 'o':
			args.out_dir = optarg;
			break;
		case 'p':
			args.prefix = optarg;
			break;
		case 'h':
			print_usage();
			exit(0);
		default:
			__VERBOSE("ERROR: Unknow option -%c!\n\n", c);
			print_usage();
			exit(1);
		}
	}

	if (optind + 2 == argc) {
		args.reference = argv[optind];
		args.in_bam = argv[optind + 1];
	} else {
		__VERBOSE("ERROR: Wrong argument format!\n\n");
		print_usage();
		exit(1);
	}

	for (i = 0, n = strlen(args.prefix); i < n; ++i)
		if (args.prefix[i] == '/')
			__ERROR("Prefix could not has '/' character");

	args.n_thread = __max(args.n_thread, 1);

	/* FIXME: check could not create output directory & add attribute */
	make_dir(args.out_dir);
}
