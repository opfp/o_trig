#!/bin/bash 

./make_prod.sh cli 

if [ $? -ne 0 ]; then 
    exit -1 
fi 

for table in tables/*; do 
    tbname=$table 
    tbname=${tbname%.*} 
    tbname=${tbname##*/} 
    for trace in traces/*; do 
        trname=$trace 
        trname=${trname%.*} 
        trname=${trname##*/} 
        
        echo $tbname"_"$trname"_q.csv"
        # echo $tbname"_"$trname".csv" 
        
        # echo "X, "$trname"(x) in radians" > trace_results/$tbname"_"$trname"_q.csv"
        # echo "X, Y" > trace_results/$tbname"_"$trname"_q.csv"
        ./cli -f $table -q -c < $trace > trace_results/$tbname"_"$trname"_q.csv" 
        python3 make_plots.py $tbname"_"$trname"_q" 

        # echo "X, "$trname"(x) in radians" > trace_results/$tbname"_"$trname".csv"
        # echo "X, Y" > trace_results/$tbname"_"$trname".csv"
        # ./cli -f $table -c < $trace > trace_results/$tbname"_"$trname".csv" 
    done 
done 