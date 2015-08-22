// in_out.c

#include <stdio.h>
#include "in_out.h"

int get_line( char *line, int lim )
{
    int c;
    char *p = line;
    while( p < line + lim-1 && (c = getchar()) != EOF && c != '\n' ) {
        *p++ = c;
    }
    *p = '\0';
    return p - line;
}

