#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h> 
#include <unistd.h>
#include <string.h> 
#include "o_trig.h"

// static char * function_names[] = {"SINE", "ARC SINE", "COSINE", "ARC COSINE", "TANGENT", "ARC TANGENT" }; 

void print_help() { 
    for ( int i = 0; i < NUM_FUNCS; i++ ) { 
        printf("%i : %s\n", i, function_names[i]); 
    } 
} 

int main(int argc, char * * argv ) { 
    char * table_fp = NULL; 
    char generate = 0; 
    unsigned inval_pts = 0; 
	int tables = 0; // By default, do everything 
    
    // char * eval = NULL; 
    char quick = 0; 
    char interactive = 0; 
    char csv = 0; 

    extern char * optarg; 
    extern int optind; 
    int c; 
    int arglen; 
    while (( c = getopt(argc, argv, "f:g:t:qic") ) != -1) { 
        switch ( c ) { 
            case 'g' : // generate [NUM POINTS] 
                generate = 1; 
                inval_pts = (int) strtoul(optarg, NULL, 10); 
                break; 
            case 'f' :  // file [FILEPATH] 
                assert( (arglen = strlen(optarg)) && "Invalid load filepath" );   
                table_fp = (char *) malloc(arglen+1); 
                strcpy(table_fp, optarg); 
                break;
			case 't' : // tables [TABLES_BITMAP] 
				if ( ! sscanf(optarg, "%i", &tables)  ) { 
					fprintf(stderr, "Invalid table bitmap value: %s\n", optarg ); 
					exit(-1); 
				} 
				break; 
            case 'q' : // quick lookups 
                quick = 1; 
                break; 
            case 'i' : // interactive mode 
                interactive = 1; 
                break;  
            case 'c' : // csv mode 
                csv = 1; 
                break; 
            // case 'e' : // evaluate one function 
            //     eval = malloc(strlen(optarg)+1); 
            //     strcpy(eval, optarg); 
            default: 
                fprintf(stderr, "Invalid option %s\n", argv[optind]); 
        } 
    } 

    if ( !tables) tables = ~0; 

    table_set * o_trig; 

    if ( generate ) { 
        o_trig = o_trig_init(tables, inval_pts);  
        if ( table_fp ) o_trig_write_file(o_trig, table_fp ); 
    } else if ( table_fp ) { 
        o_trig = o_trig_load_file(table_fp); 
    } 

    free(table_fp); 
    int func; 
    float inval; 
    int rc; 

    // if ( eval ) { 

    // } 

    if ( interactive ) {
        print_help();  
        while (1) { 
            printf("\n> "); 
            rc = scanf("%i %f", &func, &inval); 
            
            fflush(stdin); 
            if ( !rc ) continue; 

            if ( func == -1 ) return 0; 

            if ( func >= 0 && func < NUM_FUNCS && rc == 2 ) { 
                printf("%s(%f) = %f\n", function_names[func], inval, o_trig_lookup(o_trig, func, inval, quick ) ); 
            } 
            
        } 
    } 

    // // char inbuf[32] = {0}; 
   
    if ( csv  ) { 
        while (1) { 
            rc = scanf("%i %f", &func, &inval); 
            
            fflush(stdin); 
            if ( !rc ) break; 

            if ( func == -1 ) return 0; 

            if ( func >= 0 && func < NUM_FUNCS && rc == 2 ) { 
                printf("%f, %f\n", inval, o_trig_lookup(o_trig, func, inval, quick ) ); 
            } 

        } 
    } 


}  
