#!/bin/bash
set -e

RED="\033[0;31m\033[1m"
NC="\033[0m" # No Color

if [ "$BASH_VERSION" = '' ]; then
    printf "${RED}Please run by command: bash build.sh${NC}\n"
    exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${DIR}
git submodule update --init --recursive

# build htslib
DIR="$(pwd)/local/"
cd htslib && autoheader && autoconf
./configure --prefix=${DIR} --disable-lzma --disable-libcurl --disable-bz2
make
make install
cd ../
