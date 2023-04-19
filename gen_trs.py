import math
import numpy as np 

points = 1000

sine_range = np.arange(0, math.pi * 2, (math.pi * 2) / points ) 
arcsine_range = np.arange(-1,1, 2 / points ) 
tangent_range = np.arange(-0.49 * math.pi, 0.49 * math.pi, ( 0.98 * math.pi ) / points )

functions = [ ( "sine", sine_range ), ( "arc_sine", arcsine_range ) , ( "cosine" , sine_range ) , 
    ("arc_cosine", arcsine_range) , ("tangent", tangent_range), ( "arc_tangent", tangent_range ) ] 


for i, func in enumerate(functions) : 
    fout = open("traces/"+func[0]+".tr", "w+") 
    for j in func[1] : 
        fout.write("%i %f\n" % ( i, j ) ) 
    fout.close() 
