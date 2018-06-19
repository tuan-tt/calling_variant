#include "ref.h"

static double prev_time;

char **ref_load(char *file_path)
{
	char **ref = NULL;
	__VERBOSE("Loading reference ... ");

	prev_time = realtime();
	int i, id, num;
	uint32_t pos = 0;
	size_t len = 0;
	char *s = NULL, c;
	FILE *fi = fopen(file_path, "r");

	if (!fi) {
		__VERBOSE("\n");
		__PERROR("Could not open reference file");
	}

	c = getc(fi);
	if (c != '>') {
		__VERBOSE("\n");
		__ERROR("Genome file is not fasta format!");
	}
	ungetc(c, fi);

	id = -1;
	while (1) {
		num = getline(&s, &len, fi);
		if (num == EOF)
			break;
		if (s[0] == '>') {
			++id;
			if (id > 0)
				ref[id - 1][pos] = '\0';
			__REALLOC(ref, id + 2);
			continue;
		}
		if (s[num - 1] == '\n')
			--num;
		__REALLOC(ref[id], pos + num + 1);
		memcpy(ref[id] + pos, s, num);
		pos += num;
	}

	ref[id][pos] = '\0';
	ref[id + 1] = NULL;
	fclose(fi);
	__VERBOSE("%.2fs\n", realtime() - prev_time);
	return ref;
}

void ref_destroy(char **ref)
{
	int i;
	for (i = 0; ref[i] != NULL; ++i)
		free(ref[i]);
	free(ref);
}

