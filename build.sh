#!/bin/bash
set -e

RED="\033[0;31m\033[1m"
NC="\033[0m" # No Color

if [ "$BASH_VERSION" = '' ]; then
    printf "${RED}Please run by command: bash build.sh${NC}\n"
    exit 1
fi

mkdir -p bin/

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${DIR}
git submodule update --init --recursive

# build htslib
DIR="$(pwd)/local/"
cd htslib
autoheader
autoconf
./configure --prefix=${DIR} --disable-lzma --disable-libcurl --disable-bz2
make
make install
cd ../

# build samtools
cd samtools
autoheader
autoconf -Wno-syntax
./configure --without-curses
make
cp samtools ../bin/
cd ../

# build bwa
cd bwa
make
cp bwa ../bin/
cd ../

# build sambamba
if [ ! -f bin/sambamba ]; then
    wget -O sambamba.tar.gz https://github.com/biod/sambamba/releases/download/v0.6.7/sambamba_v0.6.7_linux.tar.bz2
    tar -xvjf sambamba.tar.gz
    rm -r sambamba.tar.gz
    mv sambamba bin/
fi

# build cvar
make
