#include "o_trig_priv.h" 
#include "o_trig.h"

table_set o_trig_obj = {0}; 

const function_des FUNC_DES[] = { 
//  rmin    rmax   tmin     tmax    transformer func    table           ascending 
	{NAN,   NAN,    0,      M_PI_2, &trans_sine,        TB_SINE_COS,    1 },	    // SINE  
	{-1,    1,      -1,     1,      NULL,               TB_SINE_COS,    1 }, 		// ARCSINE 
	{NAN,    NAN,   0,      M_PI_2, &trans_cosine,      TB_SINE_COS,    0 }, 	    // COS
	{-1,    1,      -1,     1,      &trans_arc_cosine,  TB_SINE_COS,    0 }, 		// ARCCOS 
	{NAN,   NAN,    0,      M_PI,   &trans_tan,         TB_TAN,         1 }, 		// TAN 
    {NAN,   NAN,    NAN,    NAN,    NULL,               TB_TAN,         1 }        	// ARCTAN
};  

const char * function_names[] = {"SINE", "ARC SINE", "COSINE", "ARC COSINE", "TANGENT", "ARC TANGENT" }; 

// PUBLIC FUNCTIONS 

/* 
	* @scope public
	* @breif initilizes the singleton o_trig_obj, either by calcuating the trig results (via 
	* 		make_tables) or loading already-generated lookup tables (via read_tables)  
	* @param table_fp filepath of lookup tables to be loaded (if applicable) 
	* @param tbs_to_load Bitmap in which tables_to_make & ( FUNC / 2 ) << 1 reflest weather the  
	* 		lookup table for FUNC will be created 
*/ 

int o_trig_init(/*char * table_fp, int tbs_to_load*/) { 
	/* 
	if ( table_fp ) { 

	} else { 
		
	} 
	*/
	o_trig_obj.points = 1000; 
	o_trig_obj.contents = 3;
	gen_lookup_tables(&o_trig_obj); 
    return 0; // just for now 
} 

/* 
    * @scope public 
    * @breif perfroms a table lookup to find an aproximation for infunc( inval ). Then, if (quick)  
    *       linearly scales the result according to the distance between the inval and closest
    *       matching x in the lookup table, and the slope of the line between infunc (aprox inval)
    *       and infunc ( aprox inval +/- 1) 
    * @param infunc The function 
    * @param inval The input value 
    * @param quick controlls weather result is sclaed to make it more accurate (for when X is not 
    *       exactly matched in the lookup table ). 
*/ 

float o_trig_lookup(enum func infunc, float inval, int quick) { 
    assert( infunc >= 0 && infunc < NUM_FUNCS && "lookup: invalid function" ); 
    // get function descriptor 
    function_des fdes = FUNC_DES[infunc]; 
    // check inval in function range 
    if ( inval < fdes.real_range_min || inval > fdes.real_range_max ) { 
        fprintf(stderr, "X %f out of range for function %s with range (%f, %f)\n", 
            inval, function_names[infunc], fdes.real_range_min, fdes.real_range_max); 
        return NAN; 
    } 
    float trans_inval = inval; 
    float mirror_about = NAN; 
    
    // if inval is outside table range, transform 
    if ( inval < fdes.table_range_min || inval > fdes.table_range_max ) { 
        (*fdes.transformer)(inval, &trans_inval, &mirror_about); 
    } 

    assert ( trans_inval >= fdes.table_range_min && trans_inval <= fdes.table_range_max && "transform failed"); 
    float * table = o_trig_obj.tables[fdes.table]; 

    // note that this is the index of X, not the result(Y)  
    int match_i = ltable_bsearch(table, trans_inval, infunc%2, fdes.ascending ); 
    int y_idx = infunc%2 ? 1 : -1; 

    float real_in = table[match_i]; 
    float result = table[match_i + y_idx]; 

    float inval_dif = real_in - inval;  

    if ( !quick && ((int) real_in * ACC_CONST) /*&& match_i >> 2 && match_i < o_trig_obj ) */ ) { 
        int next_i;
        if ( !(match_i+2< o_trig_obj.points ) && ( match_i < 2 || fdes.ascending && inval_dif > 0
            || !fdes.ascending && inval_dif < 0 ) )
        {
            next_i = match_i+2; 
        } else { 
            next_i = match_i-2; 
        } 

        float slope = (real_in - table[next_i]) / (result - table[next_i+y_idx]) ;
        result += inval_dif*slope; 
    }

    if ( ! isnan(mirror_about) ) { 
        result += 2.0 * ( mirror_about - result ); 
    } 

    return result; 
}

// PRIVATE FUNCTIONS 

/* 
	* @scope private
	* @param struct dest, The table_set which contains: 
	* 	@member contents Bitmap of the function results to record  
	* 	@member	points The number of points to generate 
	* 	@member table The destination for thegenerated lookup table  
	* @return 0 on success, else ecode  
*/ 

int gen_lookup_tables( table_set * dest ) { 
	int point_density = dest->points; 
	int tables_to_make = dest->contents; 

	// if tables_to_make is null, make them all 
	tables_to_make = tables_to_make ? tables_to_make : ~0;
	// alloc lookup tables
	int mask = 1;  
	for ( int i = 0; i < NUM_FUNCS; i++ ) { 
		if ( mask & tables_to_make ) { 
			o_trig_obj.tables[i] = malloc(point_density * 2 * sizeof(float)); 
		}
		mask <<= 1;  
	}  
 
	// make point cloud y -> x ( y = i * ( 1 / point_density ) ) 
	float c_circ_len = 0; 
	//float * point_cloud = malloc(point_density * sizeof(float));

	// (magic number) constants (should be set elsewhere in production) 
	// float accuracy = 0x100; // if ( c_dif * accuraccy >= 1 ), point is not "close enough" to being on the circle 
	
    float retry_inc_constant = 1 / ( ACC_CONST * 2 ) ; 

	// point cloud generator vals 
	float y_inc = /*1*/ .5 / point_density;  
	float px = 1;
	float py = 0;
	float x_inc = 0; 
    // float x_inc_temp;
    int undershoot;  
	float cx; 
	float cy; 

	float c_dif;  
	float hyp_len; 	

	for ( int i = 0; i < point_density; i++ ) { 
		// find point on circle at this height 
		cx = px - x_inc; 
		cy = py + y_inc; 
		
		c_dif = DIST(0,0,cx,cy) - 1.0;
        // x_inc_temp = 
         
        undershoot = 0; 
		while ( (int) ( c_dif * ACC_CONST ) ) { // while point not close enough to circle  
			assert(c_dif > 0 && "unexpected overshoot" );
            cx -= retry_inc_constant;
            undershoot++; 
			c_dif = DIST(0,0,cx,cy) - 1.0;
		}  
        printf("(%f,%f) {undershot %i times}\n", cx, cy, undershoot); 
		// with (cx, cy) a point on the circle, incriment angle 
		c_circ_len += DIST(px,py,cx,cy);
		hyp_len = DIST(0,0,cx,cy);  
		// now, with a point on the circle and it's corresponding angle, fill in function solutions ( i -> [ r, xxx(r) ] ) with xxx sine, cosine, tangent . . .  
		if ( tables_to_make & ( 1 << TB_SINE_COS ) ) { 
			o_trig_obj.tables[TB_SINE_COS][i*2] = cy / hyp_len; 
			o_trig_obj.tables[TB_SINE_COS][(i*2)+1] = c_circ_len;
    //        printf("sin(%f) = %f\n", o_trig_obj.tables[TB_SINE_COS][i*2], o_trig_obj.tables[TB_SINE_COS][(i*2)+1]); 
		} 

		if ( tables_to_make & ( 1 << TB_TAN ) ) { 
			o_trig_obj.tables[TB_TAN][i*2] = cy / cx; 
			o_trig_obj.tables[TB_TAN][(i*2)+1] = c_circ_len; 
   //         printf("tan(%f) = %f\n", o_trig_obj.tables[TB_TAN][i*2], o_trig_obj.tables[TB_TAN][(i*2)+1]); 
		} 		 

		// prepare for next point generation 
		x_inc = cx - px; 
        // x_inc += undershoot * retry_inc_constant; 
		px = cx; 
		py = cy; 
	}   
	return 0; // just for now 
}

/* 
	* @scope private 
	* @param table The lookup table to be searched 
	* @param search_val The value being searched for  
	* @param offset The allignment of the search_val. As the lookup table is populated with f(x) -> y, 
	* 		with x on even i and y on odd ie for tan: { [50] : 1 , [51] : pi/4 }. offset is 0 for 
	* 		normal function lookups, and 1 for inverse lookups.  
	* @param ascending Boolean which describes weather searched values are ascending 
	* @brief Performs a binary search of a lookup table 
	* @return index of the closest match to search_val 
*/ 

int ltable_bsearch( float * table, float search_val, int offset, char ascending) { 
	int points = o_trig_obj.points; 
    int search_inc = points / 2; 
	unsigned si = points + offset; 
	// check edge cases
	int i_high = ascending ? (points*2)-2+offset : offset;   
	int i_low = ascending ? offset : (points*2)-2+offset; 	

	if ( search_val >= table[i_high] ) { 
		return i_high; 
	} else if ( search_val <= table[i_low] ) { 
		return i_low; 
	}  
 
	char up_gz; 
	char dw_gz; 
	
	while (1) { 
		up_gz = (table[si+offset+2] - search_val) > 0; 
		dw_gz = (table[si+offset-2] - search_val) > 0; 
		
		if ( up_gz != dw_gz ) { 
			return si; 
		} 
		
		if ( ( ascending && up_gz) || (!ascending && !up_gz) ) { // if ascending and too high, or descending and too low, search lower indicies  
			si -= search_inc; 
		} else { // if ascending and too low, or descending and too high, search higher indicies  
			si += search_inc; 
		} 
		
		search_inc >>= 1; // divide search incriment by two 
	}   

} 

/* 
    * @scope private 
    * @brief Transforms input value for sine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of periodicity and symmetry of sine. for example 
    *       sin( 4pi / 3) == -1 * sin( 2pi / 3 ) 
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_ab Pointer to where the mirror result is to be stored 
*/  

void trans_sine(float x_in, float * p_x_trans, float * p_mirror_ab ) { 
    float mirror = NAN; 
    
    // first, transform into (0, 2pi) (ternary ensures fmod result is non negitive 
    float x_trans = fmod( ( x_in < 0 ) ? -1.0 * x_in : x_in , 2*M_PI ); 
    
    /* 
        TODO: This logic may be recreated with less branches. 
    */ 

    // now, if on (0, pi/2), no further transform nesecary 
    if ( x_trans <= M_PI_2 ) goto _ret; 
   
    // if on (pi/2, pi), mirror x about x=pi/2 
    if ( x_trans <= M_PI ) { 
        x_trans -= (x_trans - M_PI_2); 
        goto _ret; 
    } 
    // if on (pi, 3pi / 2 ), subtract pi from x and multiply result by negitive one 
    if ( x_trans <= 1.5 * M_PI ) { 
        x_trans -= M_PI; 
        mirror = 0; // mirror result about y=0 (multiply by negitive one) 
        goto _ret; 
    } 
    // final case (3pi /2 , 2pi ) 
    x_trans -= ( x_in - M_PI ); 
    mirror = 0; 

    _ret:; 
    *p_x_trans = x_trans; 
    *p_mirror_ab = mirror; 
} 

/* 
    * @scope private 
    * @brief Transforms input value for cosine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of the relationship cos(x) = sin(x + pi/2) 
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_ab Pointer to where the mirror result is to be stored 
*/  

void trans_cosine(float x_in, float * p_x_trans, float * p_mirror_ab) { 
	trans_sine(x_in + M_PI_2, p_x_trans, p_mirror_ab);
} 

/* 
    * @scope private 
    * @brief Transforms input value for arc_cosine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of the fact that arc_cos = arc_sine reflected about 
    *       y = pi / 4 
    * @param y_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_ab Pointer to where the mirror result is to be stored 
*/ 

void trans_arc_cosine(float y_in, float * p_x_trans, float * p_mirror_ab) { 
    *p_x_trans = y_in; 
    *p_mirror_ab = M_PI_4; 
} 

/* 
    * @scope private 
    * @brief Transforms input value for tangent function into a value which can be used to perform 
    *       a table lookup.
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_ab Pointer to where the mirror result is to be stored 
*/ 

void trans_tan(float y_in, float * p_x_trans, float * p_mirror_ab) { 
    // float x_trans = fmod( ( y_in < 0 ) ? -1.0 * y_in : y_in, M_PI ); 
    float x_trans = fmod( y_in * ( y_in < 0 ? -1 : 1 ),  M_PI ); 
  
    *p_x_trans = y_in; 
    *p_mirror_ab = x_trans < 0 ? 0 : NAN; 
} 

/* 
    * @scope private 
	* @breif Generates lookup tables for trig function results (calcu
	* @param point_den Number of points which will be used to generate the lookup tables
	* @param struct result_des Result descriptor which regulates how results are recorded 
	* @param tables_to_make Bitmap in which tables_to_make & ( FUNC / 2 ) << 1 reflest weather the lookup table 
	* 		for FUNC will be created   
*/  
