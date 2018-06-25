#include "variant.h"
#include "argument.h"
#include "ref.h"

const double EPS = 1e-6;
const int MAGIC = 5;
const int SCALE = 20;

static FILE **fo;

struct var_result_t {
	char ref_nt;
	char var_nt[4];
	int n_var;
	double phred[4];
	uint32_t ref_pos;
	double sum_ref;
	double sum[4];
	int cnt_ref;
	int cnt[4];
	int read_depth;
	int genotype;
};

/* return false prob */
static inline double phred_to_prob(int phred)
{
	if (phred == 0)
		phred = 1;
	return pow(10, -phred / 10.0);
}

void variant_init(int n_target)
{
	char file_path[BUFSZ];
	int i;

	__CALLOC(fo, n_target);
	for (i = 0; i < n_target; ++i) {
		sprintf(file_path, "%s/%d.vcf", args.temp, i);
		fo[i] = fopen(file_path, "w");
	}
}

void variant_destroy(int n_target)
{
	int i;
	for (i = 0; i < n_target; ++i)
		fclose(fo[i]);
	free(fo);
}

void variant_merge(int n_target, struct bam_inf_t *bam_inf)
{
	char file_path[BUFSZ];
	FILE *fo_res;
	int i;
	sprintf(file_path, "%s/%s.vcf", args.out_dir, args.prefix);
	fo_res = fopen(file_path, "w");
	
	fprintf(fo_res, "##fileformat=VCFv4.2\n");
	fprintf(fo_res, "##source=cvar v%s\n", VERSION);
	fprintf(fo_res, "##reference=%s\n", args.reference);

	for (i = 0; i < n_target; ++i) {
		fprintf(fo_res, "##contig=<ID=%s,length=%u>\n",
			bam_inf->b_hdr->target_name[i], bam_inf->b_hdr->target_len[i]);
	}

	fprintf(fo_res, "##FORMAT=<ID=RO,Number=1,Type=Integer,Description=\"Count of full observations of the reference haplotype.\">\n");
	fprintf(fo_res, "##FORMAT=<ID=AO,Number=A,Type=Integer,Description=\"Count of full observations of this alternate haplotype.\">\n");
	fprintf(fo_res, "##FORMAT=<ID=QR,Number=1,Type=Integer,Description=\"Reference allele quality sum in phred\">\n");
	fprintf(fo_res, "##FORMAT=<ID=QA,Number=A,Type=Integer,Description=\"Alternate allele quality sum in phred\">\n");
	fprintf(fo_res, "##FORMAT=<ID=GT,Number=1,Type=String,Description=\"Genotype\">\n");
	fprintf(fo_res, "##FORMAT=<ID=DP,Number=1,Type=Integer,Description=\"Read Depth\">\n");
	fprintf(fo_res, "#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT\tunknown\n");

	fclose(fo_res);

	/* merge all temp file to main output file */
	for (i = 0; i < n_target; ++i) {
		char temp_path[BUFSZ];
		sprintf(temp_path, "%s/%d.vcf", args.temp, i);
		append_file(file_path, temp_path, 0);
	}
}

static void output_result(int id, struct var_result_t *result)
{
	/* target name, target pos, target nt */
	fprintf(fo[id], "%s\t%d\t.\t%c\t", ref_getname(id),
		result->ref_pos + 1, result->ref_nt);
	
	/* variant nt */
	fprintf(fo[id], "%c", result->var_nt[0]);
	if (result->n_var == 2)
		fprintf(fo[id], ",%c\t", result->var_nt[1]);
	else
		fprintf(fo[id], "\t");

	/* variant phred */
	fprintf(fo[id], "%.1f", result->phred[0]);
	if (result->n_var == 2)
		fprintf(fo[id], ",%.1f\t", result->phred[1]);
	else
		fprintf(fo[id], "\t");

	/* additional info */
	fprintf(fo[id], ".\t.\tGT:DP:RO:QR:AO:QA\t");

	/* genotype */
	if (result->genotype > 0) {
		if (result->n_var == 2)
			fprintf(fo[id], "1/2:");
		else
			fprintf(fo[id], "1/1:");
	} else {
		fprintf(fo[id], "0/1:");
	}

	/* read depth, count ref, sum ref */
	fprintf(fo[id], "%d:%d:%.1f:", result->read_depth,
		result->cnt_ref, result->sum_ref);

	/* count var */
	fprintf(fo[id], "%d", result->cnt[0]);
	if (result->n_var == 2)
		fprintf(fo[id], ",%d:", result->cnt[1]);
	else
		fprintf(fo[id], ":");

	/* sum var */
	fprintf(fo[id], "%.1f", result->sum[0]);
	if (result->n_var == 2)
		fprintf(fo[id], ",%.1f", result->sum[1]);

	/* end */
	fprintf(fo[id], "\n");
}

static void normalize_result(struct var_result_t *result)
{
	int i;
	if (result->genotype > 0) {
		/* if has bb case, keep bb case only */
		for (i = 0; i < result->n_var; ++i) {
			if (!(result->genotype & (1 << i))) {
				result->n_var--;
				result->phred[i] = result->phred[result->n_var];
				result->cnt[i] = result->cnt[result->n_var];
				result->sum[i] = result->sum[result->n_var];
				result->var_nt[i] = result->var_nt[result->n_var];
			}
		}
	} else {
		/* all is ab case, we keep 1 only */
		if (result->n_var == 2) {
			result->n_var--;
			if (result->phred[1] > result->phred[0]) {
				result->phred[0] = result->phred[1];
				result->cnt[0] = result->cnt[1];
				result->sum[0] = result->sum[1];
				result->var_nt[0] = result->var_nt[1];
			}
		}
	}
}

static void process_step2(double *sum, char ref_nt, uint32_t ref_pos, int id,
			  int read_depth, int *cnt)
{
	double sum_ref, total_sum, ratio_ref, ratio_alt, min_ratio, phred;
	struct var_result_t result;
	int ref_id, i, cnt_ref;

	if (ref_nt == 'A') {
		sum_ref = sum[0];
		cnt_ref = cnt[0];
		ref_id = 0;
	} else if (ref_nt == 'C') {
		sum_ref = sum[1];
		cnt_ref = cnt[1];
		ref_id = 1;
	} else if (ref_nt == 'G') {
		sum_ref = sum[2];
		cnt_ref = cnt[2];
		ref_id = 2;
	} else if (ref_nt == 'T') {
		sum_ref = sum[3];
		cnt_ref = cnt[3];
		ref_id = 3;
	} else {
		assert(false);
	}

	result.n_var = 0;
	result.ref_nt = ref_nt;
	result.ref_pos = ref_pos;
	result.read_depth = read_depth;
	result.sum_ref = sum_ref;
	result.cnt_ref = cnt_ref;
	result.genotype = 0;

	for (i = 0; i < 4; ++i) {
		if (ref_id == i)
			continue;
		if (sum[i] <= 16)
			continue;

		/* take mean */
		total_sum = sum[i] + sum_ref;
		ratio_ref = sum_ref / total_sum;
		ratio_alt = sum[i] / total_sum;

		min_ratio = __min(ratio_alt, ratio_ref) * SCALE;

		/* calculate phred of this variant (max 60) */
		if (min_ratio <= MAGIC) {
			phred = min_ratio / MAGIC * 10;
		} else {
			phred = (min_ratio - MAGIC) / MAGIC * 50 + 10;
		}

		if (phred < 4) {
			/* no variant (aa) */
			if (ratio_alt < ratio_ref)
				continue;
			
			/* variant (bb) */
			if (result.n_var == 2)
				continue;
			result.cnt[result.n_var] = cnt[i];
			result.sum[result.n_var] = sum[i];
			result.var_nt[result.n_var] = nt4_char[i];
			result.phred[result.n_var] = 60 - phred;
			result.genotype |= (1 << result.n_var);
			result.n_var++;
		} else {
			/* variant (ab) */
			if (result.n_var == 2)
				continue;
			result.cnt[result.n_var] = cnt[i];
			result.sum[result.n_var] = sum[i];
			result.var_nt[result.n_var] = nt4_char[i];
			result.phred[result.n_var] = phred;
			result.n_var++;
		}
	}

	if (result.n_var == 0)
		return;
	normalize_result(&result);
	output_result(id, &result);
}

void variant_process(struct var_cand_t *cand_list, int cand_sz,
		     uint32_t ref_pos, char ref_nt, int id)
{
	bool is_candidate = false;
	double sum[4] = {0, 0, 0, 0};
	int i, cnt[4] = {0, 0, 0, 0};

	assert(cand_sz > 0);
	if (ref_nt == 'N')
		return;

	for (i = 0; i < cand_sz; ++i) {
		assert(cand_list[i].nt4 != 'N');
		if (cand_list[i].nt4 != ref_nt)
			is_candidate = true;
		if (cand_list[i].baseq == 255)
			cand_list[i].baseq = 0;
		if (cand_list[i].baseq > 60)
			cand_list[i].baseq = 60;

		if (cand_list[i].nt4 == 'A') {
			sum[0] += pow(2, cand_list[i].baseq / 10.0);
			cnt[0]++;
		} else if (cand_list[i].nt4 == 'C') {
			sum[1] += pow(2, cand_list[i].baseq / 10.0);
			cnt[1]++;
		} else if (cand_list[i].nt4 == 'G') {
			sum[2] += pow(2, cand_list[i].baseq / 10.0);
			cnt[2]++;
		} else {
			sum[3] += pow(2, cand_list[i].baseq / 10.0);
			cnt[3]++;
		}
	}

	if (!is_candidate)
		return;

	process_step2(sum, ref_nt, ref_pos, id, cand_sz, cnt);
}
