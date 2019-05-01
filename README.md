## Overview

<b>cvar</b> is a tool that process NGS data to calling variant.

## Install

```shell
git clone https://github.com/tuan-tt/calling_variant.git
cd cvar
bash build.sh
```

## Usage

<b>cvar</b> take BAM file as input. Then BAM file must be sorted and be indexed.
Recommend used BWA-MEM for align reads to reference.

This tool will generate ${prefix}.vcf in output directory

Pipeline to calling variant from raw reads:

```shell
bin/bwa mem -t 32 ref.fa read1.fq [read2.fq] | samtools view -Sb | samtools sort -@ 32 > $out_dir/$prefix.sorted.bam
bin/sambamba markdup -r -t 32 $out_dir/$prefix.sorted.bam $out_dir/prefix.mardup.sorted.bam
bin/cvar -t 32 -p $prefix -o $out_dir ref.fa $out_dir/prefix.mardup.sorted.bam
```

## Contacts

Please report any issues directly to the github issue tracker.

## Reference

https://hbctraining.github.io/In-depth-NGS-Data-Analysis-Course/sessionVI/lessons/02_variant-calling.html

https://arxiv.org/pdf/1207.3907.pdf

http://samtools.sourceforge.net/mpileup.shtml

http://clavius.bc.edu/~erik/CSHL-advanced-sequencing/freebayes-tutorial.html

http://www.mi.fu-berlin.de/wiki/pub/ABI/Genomics12/varcall.pdf

https://www.nature.com/articles/ng.806

https://samtools.github.io/hts-specs/SAMv1.pdf

https://samtools.github.io/hts-specs/VCFv4.2.pdf

https://software.broadinstitute.org/gatk/documentation/

https://vi.wikipedia.org/wiki/DNA
