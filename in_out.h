// in_out.h

#ifndef IN_OUT_H
  #define IN_OUT_H

#include <stdlib.h>
#include <stdbool.h>

/* get_line: reads lim-1 symbols or until EOL or EOF found. The result is
 * finished with '\0'. Returns the length of string */
int get_line( char *line, int lim );

/* open_file: wraps fopen with error-handling */
bool open_file( FILE **fp, char *file_name, char *mode );

/* same_file: using standard function realpath to check if fname1 and fname2 
 * refer to one file in the filesystem (arguments must be null-terminated 
 * strings) */
bool same_file( char * fname1, char * fname2 );

/* get_fname: waits for input; returns false on Ctrl-D */
bool get_fname( char * fname, int lim ); 

/* close_file: wraps fclose() with error handling.
 * Sets *fp to NULL and returns true if succeeded */
bool close_file( FILE ** fp );

#endif //IN_OUT_H
