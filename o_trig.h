#ifndef O_TRIG 
#define O_TRIG 

#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include <assert.h> 

// Ensure math.h constants defined 
#ifndef M_PI 
# define M_PI		3.14159265358979323846	/* pi */
#endif 
#ifndef M_PI_2 
# define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif 
#ifndef M_PI_4 
# define M_PI_4		0.78539816339744830962	/* pi/4 */ 
#endif 

// Default accuracy / generation settings 

#define TBS_TO_MAKE 3 
#define NUM_TABLES 2 
#define NUM_FUNCS 6
 
#ifndef ACC_CONST 
# define ACC_CONST .000002 
#endif 

#ifndef POINTS 
# define POINTS 2048 
#endif  

#ifndef VDEBUG  
# define VDEBUG 0 
#endif 

// usefull formulas 

// #define C_PRIME(x) ( -1.0 * (float) x ) * ( 1 / sqrt( ( -1.0 * pow(x, 2.0) ) + 1.0 ) )  
//#define DIST(x1,y1,x2,y2) ( (float) sqrt( pow( (float)x2 - (float) x1, 2) + pow( (float)y2 - (float) y1, 2) ) ) 

#define DIST(x1,y1,x2,y2) ( sqrt( pow(x2 - ( (float) x1) , 2) + pow(y2 - ( (float) y1) , 2 ) ) ) 

// quadratic formula (+) 

#define QUAD_SOLVE_P(a,b,c) ( /*(float)*/ ( (-1.0 * b) + sqrt( pow(b,2) - (4.0 * a * c) ) ) / (2.0 * a) )

//enum func{sine, cosine, tangent, arc_sine, arc_cosine, arc_tangent};
enum tables{TB_SINE_COS, TB_TAN}; 
enum func{SINE, ARC_SINE, COSINE, ARC_COSINE, TANGENT, ARC_TANGENT };  
enum units{DEGREES, RADIANS}; 

extern const char * function_names[]; 

typedef struct  { 
	int contents; // bitmap 1 << func <- weather the function has a lookup table in 
	unsigned points;
//	result_descriptor;  
	float * tables[NUM_TABLES]; 
} table_set;
 
typedef struct  { 
	float real_range_min; 
	float real_range_max;
	float table_range_min; 
	float table_range_max;   
	void (* transformer)(float, float *, float *);
	int table;   
    int ascending; 
} function_des; 

typedef struct  { 
	enum units res_units; 
	int res_scale; 
} result_descriptor; 
 
// inits 
int o_trig_init(/*char * table_fp, int tbs_to_load */); 

// basic lookup (returns result in radians) 
float o_trig_lookup(enum func infunc, float inval, int quick ); 

#endif 
