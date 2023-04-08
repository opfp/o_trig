#/bin/bash 

##extra_args="" 

##if ( $# > 1 ); then
##	exta_args=$2 
##fi  
	 

gcc o_trig.c $1.c $2 -lm --std=c11 -O0 -g -o $1 
