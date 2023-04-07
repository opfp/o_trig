#include <stdio.h> 
#include "o_trig.h"

int main() { 
	o_trig_init(); 
    printf("%f\n", o_trig_lookup(SINE, .8, 0) ); 
}  
