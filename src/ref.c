#include "ref.h"

static uint32_t *ref_len;
static char **ref_name;
static char **ref;
static int n_ref;

static void cut_off_name(char *s)
{
	for ( ; *s; ++s) {
		if (*s == ' ') {
			*s = '\0';
			break;
		}
	}
}

void ref_load(char *file_path)
{
	__VERBOSE("Loading reference ... ");

	double prev_time = realtime();
	int id, num;
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
			if (id > 0) {
				ref[id - 1][pos] = '\0';
				ref_len[id - 1] = pos;
				__REALLOC(ref[id - 1], pos + 1);
			}
			pos = 0;
			__REALLOC(ref, id + 1);
			__REALLOC(ref_len, id + 1);
			__REALLOC(ref_name, id + 1);
			ref_name[id] = strdup(s + 1);
			cut_off_name(ref_name[id]);
			ref_len[id] = 0;
			ref[id] = NULL;
			continue;
		}
		if (s[num - 1] == '\n')
			--num;
		if (pos + num + 1 >= ref_len[id]) {
			uint32_t temp = pos + num + 1;
			ref_len[id] = __round_up_32(temp) * 2;
			__REALLOC(ref[id], ref_len[id]);
		}
		memcpy(ref[id] + pos, s, num);
		pos += num;
		
	}

	ref[id][pos] = '\0';
	ref_len[id] = pos;
	__REALLOC(ref[id], pos + 1);

	n_ref = id + 1;
	fclose(fi);
	__VERBOSE("%.2fs\n", realtime() - prev_time);
}

void ref_destroy()
{
	int i;
	for (i = 0; i < n_ref; ++i) {
		free(ref[i]);
		free(ref_name[i]);
	}
	free(ref_len);
	free(ref_name);
	free(ref);
}

int ref_getlen(int id)
{
	if (id >= 0 && id < n_ref)
		return ref_len[id];
	else
		assert(false);
}

int ref_getid(char *s)
{
	int i;
	for (i = 0; i < n_ref; ++i)
		if (strcmp(ref_name[i], s) == 0)
			return i;
	assert(false);
}

char *ref_getseq(int id)
{
	if (id >= 0 && id < n_ref)
		return ref[id];
	else
		assert(false);
}

char *ref_getname(int id)
{
	if (id >= 0 && id < n_ref)
		return ref_name[id];
	else
		assert(false);
}
