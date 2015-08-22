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

#endif //IN_OUT_H
 
