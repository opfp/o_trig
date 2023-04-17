#ifndef O_TRIG_PRIV 
#define O_TRIG_PRIV 

#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include <assert.h> 
#include <unistd.h> 
#include "o_trig.h" // circular inclusion? 

// #include <libconfig.h> 

// Default accuracy / generation settings 

#define TBS_TO_MAKE 3   
//#define NUM_TABLES 3 
//#define NUM_FUNCS 6
 
#ifndef ACC_CONST 
# define ACC_CONST .000002 
#endif 

#ifndef POINTS 
# define POINTS 0x800 
#endif  

#ifndef VDEBUG  
# define VDEBUG 0 
#endif 

// USEFULL FORMULAS 

// distance formula 
#define DIST(x1,y1,x2,y2) ( sqrt( pow(x2 - ( (float) x1) , 2) + pow(y2 - ( (float) y1) , 2 ) ) ) 

// quadratic formula (+) 
#define QUAD_SOLVE_P(a,b,c) ( /*(float)*/ ( (-1.0 * b) + sqrt( pow(b,2) - (4.0 * a * c) ) ) / (2.0 * a) )

// reflect x about y=? 
#define REFLECT_Y(x,y) ( -1.0 * ( (float) x - y ) + (float) y ) 

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

//enum func{sine, cosine, tangent, arc_sine, arc_cosine, arc_tangent};

typedef struct  { 
	float real_range_min; 
	float real_range_max;
	float table_range_min; 
	float table_range_max;   
	void (* transformer)(float, float *, float *, float *);
	int table;   
    char ascending; 
    char half_size; 
} function_des; 

// typedef struct transform_result{ // could just pack into a double, but that's not as portable 
//     float lookup_val; 
//     float result_mirror_about; 
// } ; 

int gen_lookup_tables( table_set * dest ); 
int ltable_bsearch( table_set * o_trig_obj, float * table, float search_val, int offset, char ascending); //, char half_size);  

// Transformers 
void trans_sine(float x_in, float * x_trans, float * mirror_y1, float * mirror_y2 ); 
void trans_arc_sine(float x_in, float * x_trans, float * mirror_ab, float * mirror_y2 ); 
void trans_cosine(float x_in, float * x_trans, float * mirror_ab, float * mirror_y2); 
void trans_arc_cosine(float, float * x_trans, float * mirror_ab, float * mirror_y2); 
void trans_tan(float x_in, float * x_trans, float * mirror_ab, float * mirror_y2); 
// void trans_arc_tan(float x_in, float * x_trans, float * mirror_ab, float * mirror_y2); 


#endif 
