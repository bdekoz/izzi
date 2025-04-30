#!/usr/bin/bash

CCFILE=$1
#OFILE=`echo $CCFILE | sed 's/.cc/.wasm/g'`
OFILE=`echo $CCFILE | sed 's/.cc/.js/g'`

emcc -std=c++20 -I../src -I/usr/include/rapidjson ${CCFILE} -o ${OFILE}
