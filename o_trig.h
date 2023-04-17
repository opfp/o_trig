#ifndef O_TRIG 
#define O_TRIG 
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

//enum func{sine, cosine, tangent, arc_sine, arc_cosine, arc_tangent};
enum tables{TB_SINE_COS, TB_TAN, TB_POINTS, NUM_TABLES }; 
enum func{SINE, ARC_SINE, COSINE, ARC_COSINE, TANGENT, ARC_TANGENT, NUM_FUNCS };  
enum error{E_NO_MEM, E_OUT_OF_BOUNDS, E_BAD_FUNC}; 
// enum units{DEGREES, RADIANS}; 

extern const char * function_names[]; 

typedef struct  { 
	int contents; // bitmap 1 << func <- weather the function has a lookup table in 
	unsigned points;  
	float * tables[NUM_TABLES]; 
    enum error edes; 
} table_set;
 
// typedef struct  { 
// 	enum units res_units; 
// 	int res_scale; 
// } result_descriptor; 
 
// inits 
table_set * o_trig_init(char * conf_st, int tbs_to_load ); 
table_set * o_trig_grow_table_set(table_set *, int factor); 

// basic lookup (returns result in radians) 
float o_trig_lookup(table_set * o_trig_obj, enum func infunc, float inval, int quick ); 

#endif 
