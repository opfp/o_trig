#include <stdio.h> 
#include "o_trig.h"

int main() { 
    // printf("%f\n", QUAD_SOLVE_P(1, M_SQRT2, -1 + .5 + pow( (M_SQRT2/2.0) + 0.000345,2) ) ); 
    // printf("%f\n",  DIST(0,0,5,8)); 
	o_trig_init(); 
    printf("%f\n", o_trig_lookup(SINE, 0.570075, 1) ); 
}  
