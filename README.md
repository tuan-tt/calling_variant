## Overview

<b>cvar</b> is a tool that process NGS data to calling variant.

## Install

```shell
git clone https://github.com/tuan-tt/calling_variant.git
cd cvar
bash build.sh
make
```

## Usage

<b>cvar</b> take raw reads (fastq format) as input. Then reads will be align to reference by BWA-MEM. After that we using sambaba to mark duplicate reads.
Finally the BAM file will be used to calling variant.

<b>cvar</b> will generate some file in output directory:

- ${prefix}.bam : the BAM file after remomve duplicate reads.
- ${prefix}.vcf : variant calling result.
- ${prefix}.log : log file.


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
