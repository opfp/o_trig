import pandas as pd 
from matplotlib import pyplot as plt
import csv 
import sys 

def main(): 
    if (len(sys.argv) < 2 ): 
        return 

    trname = sys.argv[1] 
   
    # tangent = False 
    # if ( "tangent" in trname and "arc" not in trname ): 
    #     # print("tan") 
    #     tangent = True 

    x = []
    y = []

    with open("trace_results/"+trname+".csv",'r') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row[0]))
            y.append(float(row[1]))


    x_min = min(x) 
    y_min = min(y) 
    x_max = max(x) 
    y_max = max(y) 

    # x_r_sz = x_max - x_min 
    # y_r_sz = y_max - y_min 

    # if ( x_r_sz > 5*y_r_sz ): 
    #     plt.xscale("log") 
    # else: 
    plt.xlim(x_min, x_max ) 

    # if ( y_r_sz > 5*x_r_sz ): 
    #     plt.yscale("log") 
    # else : 
    plt.ylim(y_min, y_max ) 

    # plt.axis("square") 

    # plt.rcParams["figure.autolayout"] = True 
    plt.plot(x,y, label=trname) 

    plt.savefig("plots/" + trname + ".pdf", format="pdf", bbox_inches="tight" ) 

if __name__ == "__main__" : 
    main() 


# plt.rcParams["figure.autolayout"] = True


# plt.show() 