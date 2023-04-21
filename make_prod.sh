#/bin/bash 

# make for production, with no debuginfos and full optimizations

gcc o_trig.c $1.c $2 -lm --std=c11 -O3 -o $1 
