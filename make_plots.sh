#!/bin/bash 

echo "./make_prod.sh cli" 
./make_prod.sh cli 

if [ $? -ne 0 ]; then 
    exit -1 
fi 

for trname in trace_results/*_q.csv ; do
#    trname=$trace_res  
    trname=${trname%.*} 
    trname=${trname##*/}
	echo $trname 
 	python3 make_plots.py $trname  
done 

