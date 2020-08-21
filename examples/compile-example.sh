#!/usr/bin/bash

FILE=$1
g++ -std=gnu++17 -g -O2 -I../src/ $FILE
