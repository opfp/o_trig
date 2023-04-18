import pandas as pd 
from matplotlib import pyplot as plt
import csv 
import sys 

def main(): 
    if (len(sys.argv) < 2 ): 
        return 

    trname = sys.argv[1] 


    x = []
    y = []

    with open("trace_results/"+trname+".csv",'r') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row[0]))
            y.append(float(row[1]))

    plt.rcParams["figure.figsize"] = [7.00, 3.50]
    plt.plot(x,y, label="SINE(X)") 
    plt.savefig("plots/" + trname + ".pdf", format="pdf", bbox_inches="tight" ) 

if __name__ == "__main__" : 
    main() 


# plt.rcParams["figure.autolayout"] = True


# plt.show() 