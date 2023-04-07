#include <stdio.h> 
#include "o_trig.h"

// static char * function_names[] = {"SINE", "ARC SINE", "COSINE", "ARC COSINE", "TANGENT", "ARC TANGENT" }; 

void print_help() { 
    for ( int i = 0; i < NUM_FUNCS; i++ ) { 
        printf("%i : %s\n", i, function_names[i]); 
    } 
} 

int main() { 
	o_trig_init(); 
    // printf("%f\n", o_trig_lookup(SINE, .8, 0) );
    printf("Interactive testing mode, input function id followed by input to receive result in radians\n"); 
    // printf("Input -1 for help, -2 to exit\n"); 
    print_help(); 

    // char inbuf[32] = {0}; 
    int func; 
    float inval; 
    int rc; 

    while (1) { 
        printf("\n> "); 
        rc = scanf("%i %f", &func, &inval); 
        // printf("%i %f (%i)\n", func, inval, rc); 
        
        fflush(stdin); 
        if ( !rc ) continue; 

        if ( func >= 0 && func < NUM_FUNCS && rc == 2 ) { 
            printf("%s(%f) = %f\n", function_names[func], inval, o_trig_lookup(func, inval, 0 ) ); 
        } 
        
        // else if ( func == -2 ) { 
        //     break; 
        // } else if ( func == -1 ) { 
        //     print_help(); 
        // } 
    } 
}  
