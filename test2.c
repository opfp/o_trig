#include <stdlib.h> 
#include <stdio.h>

#include "o_trig_priv.h" 

// test transformation functions 
int main() { 
    float x_in = 8.3; 
	float x_trans; 
	float mirror_ab; 
	trans_sine(x_in, &x_trans, &mirror_ab ); 
	printf("%f -> %f mirror: %f\n", x_in, x_trans, mirror_ab); 	
}  
