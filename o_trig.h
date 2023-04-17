#ifndef O_TRIG 
#define O_TRIG 

enum func{SINE, ARC_SINE, COSINE, ARC_COSINE, TANGENT, ARC_TANGENT, NUM_FUNCS };  
enum error{E_NO_MEM, E_OUT_OF_BOUNDS, E_BAD_FUNC}; 
enum tables{TB_SINE_COS, TB_TAN, TB_POINTS, NUM_TABLES }; 

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
table_set * o_trig_init(int tbs_to_load, unsigned points ); 
table_set * o_trig_load_file(char * fp); 
void o_trig_write_file( table_set * obj, char * fp ); 
table_set * o_trig_grow_table_set(table_set *, int factor); 

// destructor 
void o_trig_free (table_set * obj ); 

// basic lookup (returns result in radians) 
float o_trig_lookup(table_set * o_trig_obj, enum func infunc, float inval, int quick ); 

#endif 
