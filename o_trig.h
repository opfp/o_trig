#ifndef O_TRIG 
#define O_TRIG 

#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include "o_trig_priv.h"  

#define NUM_TABLES 2 
#define NUM_FUNCS 6 

#define DIST(x1,y1,x2,y2) ( (float) sqrt( exp( (float)x1 - (float)x2 ) + exp( (float)y1 - (float)y2 ) ) )  

//enum func{sine, cosine, tangent, arc_sine, arc_cosine, arc_tangent};
enum tables{TB_SINE_COS, TB_TAN}; 
enum func{SINE, ARC_SINE, COSINE, ARC_COSINE, TANGENT, ARC_TANGENT };  
enum units{DEGREES, RADIANS}; 
// enum errors{OUT_OF_BOUNDS, 

typedef struct table_set { 
	int contents; // bitmap 1 << func <- weather the function has a lookup table in 
	unsigned points;
//	result_descriptor;  
	float * tables[NUM_TABLES]; 
};
 
typedef struct function_des { 
	float real_range_min; 
	float real_range_max;
	float table_range_min; 
	float table_range_max;   
	void (* transformer)(struct transform_result *);
	int table;   
    int ascending; 
}; 

typedef struct result_descriptor { 
	enum units res_units; 
	int res_scale; 
}; 
 
// inits 
int o_trig_init(char * table_fp, int tbs_to_load ); 

// lookups (default, in radians)  
float o_trig_lookup(enum func infunc, float inval); 
float o_trig_quick_lookup(enum func infunc, float inval); 

//float o_trig_lookup_inverse(enum func, float result);
// lookups (result type specified) 
//void o_trig_lookup_desc(
 
/*
double o_trig_lookup_d(enum func infunc, double ratio); 
double o_trig_lookup_inverse_d(enum func, double result); 
*/ 

#endif 
