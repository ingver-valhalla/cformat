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

bool open_file( FILE **fp, char *file_name, char *mode )
{
    if( fp == NULL ) {
        return false;
    }
    *fp = fopen( file_name, mode );
    if( *fp == NULL ) {
        fprintf( stderr, "Cannot open file: %s\n", file_name );
        return false;
    }
    return true;
}
