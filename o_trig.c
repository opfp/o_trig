#include "o_trig_priv.h" 
#include "o_trig.h"

// table_set o_trig_obj = {0}; 

const function_des FUNC_DES[] = { 
//  rmin    rmax   tmin     tmax    transformer func    table           ascending   always trans 
	{NAN,   NAN,    0.0,    M_PI_2, &trans_sine,        TB_SINE_COS,    1,          0}, 	    // SINE  
	{-1.0,  1.0,    0.0,    1.0,    &trans_arc_sine,    TB_SINE_COS,    1,          0 }, 		// ARCSINE 
	{NAN,   NAN,    0.0,    M_PI_2, &trans_cosine,      TB_SINE_COS,    1,          1 }, 	    // COS
	{-1.0,  1.0,    0.0,    1.0,    &trans_arc_cosine,  TB_SINE_COS,    1,          1 }, 		// ARCCOS 
	{NAN,   NAN,    0.0,    M_PI_2, &trans_tan,         TB_TAN,         1,          0 }, 		// TAN 
    {NAN,   NAN,    0.0,    NAN,    &trans_tan,         TB_TAN,         1,          0 }        	// ARCTAN
};  

const char * function_names[] = {"SINE", "ARC SINE", "COSINE", "ARC COSINE", "TANGENT", "ARC TANGENT" }; 

// PUBLIC FUNCTIONS 

/* 
	* @scope public
	* @breif initilizes the singleton o_trig_obj, either by calcuating the trig results (via 
	* 		make_tables) or loading already-generated lookup tables (via read_tables)  
	* @param table_fp filepath of lookup tables to be loaded (if applicable) 
	* @param tbs_to_load Bitmap in which tables_to_make & ( FUNC / 2 ) << 1 reflects weather the  
	* 		lookup table for FUNC will be created 
*/ 

table_set * o_trig_init(/*char * conf_st,*/  int tbs_to_load, unsigned points ) { 
	table_set * o_trig_obj = malloc(sizeof(table_set) ); 
	o_trig_obj->points = points;//0; 
	o_trig_obj->contents = tbs_to_load;
	gen_lookup_tables(o_trig_obj); 
    
    return o_trig_obj; // just for now 
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

float o_trig_lookup(table_set * o_trig_obj, enum func infunc, float inval, int quick) { 
    assert( (infunc >= 0) && (infunc < NUM_FUNCS) && "lookup: invalid function" ); 
    // get function descriptor 
    function_des fdes = FUNC_DES[infunc]; 
    // check inval in function range 
    if ( inval < fdes.real_range_min || inval > fdes.real_range_max ) { 
        fprintf(stderr, "X %f out of range for function %s with range (%f, %f)\n", 
            inval, function_names[infunc], fdes.real_range_min, fdes.real_range_max); 
        return NAN; 
    } 
    float trans_inval = inval; 
    float mir_1 = NAN;
    float mir_2 = NAN; 
    
    // if inval is outside table range, transform 
    if ( fdes.always_transform || inval < fdes.table_range_min || inval > fdes.table_range_max ) { 
        (*fdes.transformer)(inval, &trans_inval, &mir_1, &mir_2); //, &y_mirror); 
    } 

    assert( ( isnan(fdes.table_range_min) || trans_inval >= fdes.table_range_min) && 
            ( isnan(fdes.table_range_max) || trans_inval <= fdes.table_range_max) && 
            "transform failed" ); 

    float * table = o_trig_obj->tables[fdes.table]; 

    // note that this is the index of X, not the result(Y) ( in the case of inverses, the return is Y ) 
    int match_i = ltable_bsearch(o_trig_obj, table, trans_inval, infunc%2, fdes.ascending); //, fdes.half_size ); 
    // int y_idx = infunc%2 ? 1 : -1; 
    int y_idx = infunc%2 ? -1 : 1; 

    float real_in = table[match_i]; 
    float result = table[match_i + y_idx]; 

    float inval_dif; //  = real_in - inval; if +, real_in > inval   

    if ( !quick && ( fabs(inval_dif = real_in - inval) > ACC_CONST ) ) { 
        int adj_i = inval_dif > 0 && fdes.ascending || inval_dif < 0 && !fdes.ascending ? match_i+2 : match_i-2; 

        if ( adj_i < 0 ) { 
            adj_i += 4; 
        } else if ( adj_i >= o_trig_obj->points*2 ) { 
            adj_i -= 4; 
        } 

        float xdif = real_in - table[adj_i]; 
        float ydif = result - table[adj_i+y_idx]; 

        float slope = ydif / xdif; 

        result += inval_dif*slope; 
 
    }

    // _done_adj:; 

    if ( ! isnan(mir_1) ) { 
		result = REFLECT_Y(result, mir_1); 
    } 

    if ( ! isnan(mir_2) ) { 
		result = REFLECT_Y(result, mir_2); 
    }

    return result; 
}

/* 
	* @scope public 
	* @brief loads lookup table into memory to be used to evaulate trig funtions 
	* @param fp path to the lookup table on disk 
*/ 

table_set * o_trig_load_file( char * fp ) { 
	FILE * FP = fopen(fp, "r");
    if ( !FP ) { 
        fprintf(stderr, "Cannot open table %s\n", fp); 
        exit(-4); 
    } 

	u_int32_t points; 
    u_int32_t tables; 
    int ecode; 

    if ( fread(&tables, 4, 1, FP) != 1 || fread( &points,4, 1, FP) != 1 ) { 
        fprintf(stderr, "Invalid lookup table file: %s\n", fp); 
        exit(-2); 
    } 

    table_set * ret = malloc( sizeof( table_set));

    ret->points = points; 
    ret-> contents = tables; 

    char mask = 1;  
    for ( int i = 0; i < NUM_TABLES; i++ ) { 
        if ( mask & tables ) { 
            ret->tables[i] = malloc(points * 2 * sizeof(float) );
            assert(ret->tables[i] && "Malloc fail" ); 
            if ( fread(ret->tables[i], sizeof(float), points*2, FP ) != 2*points ) { 
                fprintf(stderr, "Error reading in lookup table %i\n", i ); 
                exit(-3); 
            } 
        } 
        mask <<= 1; 
    }  

    fgetc(FP); 
    if ( ! feof(FP) ) { 
        fprintf(stderr, "Malformed lookup table file %s\n", fp); 
        // exit(-3); 
    } 

    return ret; 
} 

/* 
	* @scope public 
	* @brief loads Saves generated lookup table to disk for future use 
    * @param obj the table object to be saved  
	* @param fp path to the lookup table on disk 
*/ 

void o_trig_write_file( table_set * obj, char * fp ) { 
    // if ( access(fp, F_OK ) ) { 
    //     fprintf(stderr, "File %s already exists\n", fp); 
    //     exit( -4 ); 
    // } 

    FILE * FP = fopen(fp, "w+"); 
    if ( !FP ) { 
        fprintf(stderr, "Cannot creaet new file %s\n", fp); 
    } 

    fwrite(&obj->contents, 4, 1, FP); 
    fwrite(&obj->points, 4, 1, FP); 

    int mask = 1; 
    for ( int i = 0; i < NUM_TABLES; i++ ) { 
        if ( mask & obj->contents ) { 
            assert( fwrite( obj->tables[i], sizeof(float), 2*obj->points, FP) == 2*obj->points && "Write failed" ); 
        } 
        mask <<= 1; 
    } 

    // fflush(FP); 
    fclose(FP); 

} 

/* 
    * @scope public 
    * @breif frees all memory allocated for this o_trig obj 
    * @param obj the o_trig_obj to be freed 
*/ 

void o_trig_free( table_set * obj ) { 
    for( int i = 0; i < NUM_TABLES; i++ ) { 
        if ( obj->tables[i] ) free(obj->tables[i]); 
    } 
    free(obj); 
} 

/* 
    * @scope public  
    * @brief Saves lookup table as csv file 
    * @param Table the table to be saved 
    * @param Outpath The path of the .csv out 
*/ 

// int save_csv( float * TABLE, char * outpath ) { 

// } 

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
	int points = dest->points; 
	int tables_to_make = dest->contents; 

	// if tables_to_make is null, make them all 
	tables_to_make = tables_to_make ? tables_to_make : ~0;
	// alloc lookup tables
	int mask = 1;  
	for ( int i = 0; i < NUM_FUNCS; i++ ) { 
		if ( mask & tables_to_make ) { 
			dest->tables[i] = malloc(points * 2 * sizeof(float)); 
		}
		mask <<= 1;  
	}  
 
	// make point cloud y -> x ( y = i * ( 1 / point_density ) ) 

    /* 
     formula of our circle: 1 = x^2 + y^2 
     We can use the quadratic formula to find the next point which satisfies the equation 
     (after incrimenting either X or Y, in this case X) 
    */ 

   float px = 1; 
   float py = 0; 

   float x_inc_per_point = 1.0 / points; 

   if ( VDEBUG ) printf("x_inc_per_point : %f\n", x_inc_per_point); 

   float cx; 
   float cy; 
   float hyp_len; 
   float arc_len = 0; //M_PI_2; 

   

   char calc_sincos = tables_to_make & ( 1 << TB_SINE_COS ); 
   char calc_tan = tables_to_make & ( 1 << TB_TAN ); 
   char rec_points = tables_to_make & ( 1 << TB_POINTS ); 
//    int start_calc_tan = (points/2); 
    
    int i = 0; 
    while ( i < points ) { 
        cx = px - x_inc_per_point;  
        cy = QUAD_SOLVE_P(1,0, (pow(cx,2) - 1.0) ); // use quadratic formula to find next point on circle 
        
        hyp_len = DIST(0,0,cx,cy); 
        // if ( VDEBUG ) printf("hyp_len %f\n", hyp_len); 
        
        // assert( hyp_len - 1.0 < .0000002 && hyp_len - 1.0 > -.0000002 ); 
        if ( !( hyp_len - 1.0 < .0000002 && hyp_len - 1.0 > -.0000002 ) ) { 
            if ( VDEBUG ) printf("failed at i= %i (points = %i)\n", i, points); 
            break; 
        } 

        // hyp_len = DIST( (px), (py), (cx), (cy) ); 

        // printf("point distance: %f\n", hyp_len); 

        arc_len += DIST(px, py, cx, cy ); 
		
        if ( calc_sincos ) { 
		    dest->tables[TB_SINE_COS][i*2] = arc_len; // theta 
            dest->tables[TB_SINE_COS][(i*2)+1] = cy; // sine(theta) 
            if ( VDEBUG ) printf("%i: sin(%f) = %f\n", i, dest->tables[TB_SINE_COS][i*2], dest->tables[TB_SINE_COS][(i*2)+1]); 
        } 

        if ( calc_tan ) { 
		    dest->tables[TB_TAN][i*2] = arc_len; // theta 
            dest->tables[TB_TAN][(i*2)+1] = cy / cx; // tan(theta) 
            if ( VDEBUG ) printf("%i: tan(%f) = %f\n", i, dest->tables[TB_TAN][i*2], dest->tables[TB_TAN][(i*2)+1]); 
        } 

        if ( rec_points ) { 
            dest->tables[TB_POINTS][i*2] = cx; 
            dest->tables[TB_POINTS][(i*2)+1] = cy; 
            if ( VDEBUG ) printf("%f, %f\n", cx, cy);  
        } 

        px = cx; 
        py = cy; 
        ++i; 
        // if ( !calc_tan && i == start_calc_tan && ( tables_to_make & ( 1 << TB_TAN ) ) ) { 
        //     if (VDEBUG) printf("Start calculating tan at %i (points %i)\n", i, points ); 
        //     calc_tan = 1; 
        // }  
	} 

    dest->points=i; // don't accound for points that were not generated 
    // points >>=1; // ensure points %2 == 0 (even if we lose a datapoint) 
    // points <<=1; 

    if ( VDEBUG ) printf("arc len innacuracy (should be 0): %f\n", arc_len - M_PI_2 ); 

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

int ltable_bsearch(  table_set * o_trig_obj, float * table, float search_val, int offset, char ascending) { 
	int points = o_trig_obj->points; 
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

    float up_dif; 
    float dw_dif; 
	
	while ( search_inc /*> 1*/ ) { 
		up_gz = ( up_dif = table[si+2] - search_val) > 0; // find the parity of i+1 and i-1 
		dw_gz = ( dw_dif = table[si-2] - search_val) > 0; // (really i+2 / i-2 because of the offset) 
		
        // if ( VDEBUG ) printf("%i\n", si ); 

		if ( up_gz != dw_gz ) { // a parity change between i-1 and i+1 means we've hit our mark 
			int closer_up_dw = fabs(up_dif) > fabs(dw_dif) ? si-2 : si+2; 
            return fabs(table[si] - search_val ) > fabs(table[closer_up_dw] - search_val) ? closer_up_dw : si; 
            // return si; 
		} 
		
		if ( ( ascending && up_gz) || (!ascending && !up_gz) ) { // if ascending and too high, or descending and too low, search lower indicies  
			si -= search_inc; 
		} else { // if ascending and too low, or descending and too high, search higher indicies  
			si += search_inc; 
		} 
		
		search_inc >>= 1; // divide search incriment by two 
	}  

    assert ( 0 && "search failed");  

}

/* 
    * @scope private 
    * @brief Transforms input value for sine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of periodicity and symmetry of sine. for example 
    *       sin( 4pi / 3) == -1 * sin( 2pi / 3 ) 
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_y1 Pointer to where the mirror result is to be stored (about y=?)  
	* @param p_mirror_y2 Is the second mirror result (Always set to NAN, only taken for 
			compatibility) with other trans functions 
*/  

void trans_sine(float x_in, float * p_x_trans, float * p_mirror_y1, float * p_mirror_y2 ) { 
    float mir_1 = NAN; 
    
    // first, transform into (0, 2pi) (abs ensures fmod result is non negitive) 
    float x_trans = fmod( fabs(x_in) , 2*M_PI ); 
    
    /* 
        TODO: This logic may be recreated with less branches. 
    */ 

    // now, if on (0, pi/2), no further transform nesecary 
    if ( x_trans <= M_PI_2 ) goto _ret; 
   
    // if on (pi/2, pi), mir_1 x about x=pi/2 
    if ( x_trans <= M_PI ) { 
        x_trans -= 2.0 * (x_trans - M_PI_2); 
        goto _ret; 
    } 
    // if on (pi, 3pi / 2 ), subtract pi from x and multiply result by negitive one 
    if ( x_trans <= 1.5 * M_PI ) { 
        x_trans -= M_PI; 
        mir_1 = 0; // mir_1 result about y=0 (multiply by negitive one) 
        goto _ret; 
    } 
    // final case (3pi /2 , 2pi ) 
    x_trans -= 2.0 * ( x_in - M_PI ); 
    mir_1 = 0; // mir_1 result about y=0 (multiply by negitive one) 

    _ret:; 
    *p_x_trans = x_trans; 
    *p_mirror_y1 = mir_1;
 	*p_mirror_y2 = NAN; 
} 

/* 
    * @scope private 
    * @brief Transforms input value for sine function into a value which may be used to perform a 
    *       table lookup. 
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_y1 Pointer to where the mirror result is to be stored
   	* @param p_mirror_y2 Is the second mirror result (Always set to NAN, only taken for 
	*		compatibility) with other trans functions  

*/ 

void trans_arc_sine( float x_in, float * p_x_trans, float * p_mirror_y1, float * p_mirror_y2 ) { 
    float trans_x = x_in; 
    float mir_1 = NAN; 
    
    if ( x_in < 0 ) { 
        trans_x = 0.0 - x_in; 
        mir_1 = 0.0; 
    } 

    *p_x_trans = trans_x; 
    *p_mirror_y1 = mir_1;
 	*p_mirror_y2 = NAN; 
} 

/* 
    * @scope private 
    * @brief Transforms input value for cosine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of the relationship cos(x) = sin(x + pi/2) 
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_y1 Pointer to where the mirror result is to be stored
	* @param p_mirror_y2 Is the second mirror result (Always set to NAN, only taken for 
	*		compatibility) with other trans functions  
*/  

void trans_cosine(float x_in, float * p_x_trans, float * p_mirror_y1, float * p_mirror_y2) { 
	trans_sine(x_in + M_PI_2, p_x_trans, p_mirror_y1, p_mirror_y2);
} 

/* 
    * @scope private 
    * @brief Transforms input value for arc_cosine function into a value which can be used to perform 
    *       a table lookup. Takes advantage of the fact that arc_cos = arc_sine reflected about 
    *       y = pi / 4 
    * @param y_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_y1 Pointer to where the mirror result is to be stored
	* @param p_mirror_y2 Is the second mirror result, which is needed for arc cosine lookups 
	* 		where x < 0.  
*/ 

void trans_arc_cosine(float x_in, float * p_x_trans, float * p_mirror_y1, float * p_mirror_y2) { 
    float trans_x = x_in; 
    float mir_1 = M_PI_4;
	float mir_2 = NAN;  

    if ( x_in < 0 ) { 
        trans_x = 0.0 - x_in;
		mir_2 = M_PI_2;  
    } 

    *p_x_trans = trans_x; 
    *p_mirror_y1 = mir_1;
	*p_mirror_y2 = mir_2;  
} 

/* 
    * @scope private 
    * @brief Transforms input value for tangent function into a value which can be used to perform 
    *       a table lookup.
    * @param x_in The input value 
    * @param p_x_trans Pointer to where the result (x after transformation) is to be stored 
    * @param p_mirror_y1 Pointer to where the mirror result is to be stored 
*/ 

void trans_tan(float x_in, float * p_x_trans, float * p_mirror_y1, float * p_mirror_y2) { 
    // float x_trans = fmod( ( y_in < 0 ) ? -1.0 * y_in : y_in, M_PI ); 
    float mirror_y1 = NAN; 
    float x_trans = x_in + M_PI_2; 
    x_trans = fmod( x_trans,  M_PI ); 
    x_trans -= M_PI_2; 
    
    if ( x_trans < 0 ) { 
        mirror_y1 = 0; 
        x_trans *= -1.0; 
    } 
  
    *p_x_trans = x_trans; 
    *p_mirror_y1 = mirror_y1;
	*p_mirror_y2 = NAN;  
} 
 