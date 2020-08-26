#!/usr/bin/env bash

for i in ./*.cc;
do
    IFILE="$i"
    echo "$i"

    ../scripts/compile-source.sh "$i";
done
