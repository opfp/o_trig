#ifndef O_TRIG_PRIV 
#define O_TRIG_PRIV 

// typedef struct transform_result{ // could just pack into a double, but that's not as portable 
//     float lookup_val; 
//     float result_mirror_about; 
// } ; 

// Transformers 
void transform_result * trans_sine(float x_in, float * x_trans, float * mirror_ab ); 
void transform_result * trans_cosine(float x_in, float * x_trans, float * mirror_ab); 
void transform_result * trans_tan(float x_in, float * x_trans, float * mirror_ab); 
void transform_result * trans_arc_cosine(float, float * x_trans, float * mirror_ab y_in); 

#endif 
