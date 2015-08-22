// in_out.c

#include <string.h>
#include <limits.h>
#ifndef PATH_MAX
  #define PATH_MAX 1024
#endif // PATH_MAX
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

bool same_file( char * fname1, char * fname2 )
{
	char buf1[PATH_MAX];
	char buf2[PATH_MAX];
	char * real1;
	char * real2;
	
	real1 = realpath( fname1, buf1 );
	real2 = realpath( fname2, buf2 );

	if( !strcmp( real1, real2 ) ) {
		return true;
	}
	return false;
}
