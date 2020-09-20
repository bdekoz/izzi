#!/usr/bin/bash

CCFILE=$1
# The input file to compile, the output filename
CCFILE=$1
if [ ! -n "$CCFILE" ]; then
    echo "Source file argument not supplied, exiting";
    exit 1;
fi

WARNF="-Werror -Wfatal-errors -Wall -Wextra -Wunused -Wno-deprecated-declarations"
COMPILEF="-std=gnu++17 -O2 -g -march=native"

EXEFILE=`echo $CCFILE | sed 's/.cc/.exe/g'`

g++ $WARNF $COMPILEF -I../src/ $CCFILE -o ${EXEFILE}
