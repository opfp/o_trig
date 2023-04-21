#/bin/bash 

# make for testing, with no optimizations and full debuginfos 

gcc o_trig.c $1.c $2 -lm --std=c11 -O0 -g -o $1 
