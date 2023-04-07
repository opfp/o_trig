#ifndef O_TRIG_PRIV 
#define O_TRIG_PRIV 

#include "o_trig.h" // circular inclusion? 

// typedef struct transform_result{ // could just pack into a double, but that's not as portable 
//     float lookup_val; 
//     float result_mirror_about; 
// } ; 

int gen_lookup_tables( table_set * dest ); 
int ltable_bsearch( float * table, float search_val, int offset, char ascending); 

// Transformers 
void trans_sine(float x_in, float * x_trans, float * mirror_ab ); 
void trans_cosine(float x_in, float * x_trans, float * mirror_ab); 
void trans_tan(float x_in, float * x_trans, float * mirror_ab); 
void trans_arc_cosine(float, float * x_trans, float * mirror_ab); 

#endif 
