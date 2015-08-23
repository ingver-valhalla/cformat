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

	if( !realpath( fname1, buf1 ) ) {
		fprintf( stderr, "Can't find file: %s\n", fname1 );
		exit( EXIT_FAILURE );
	}
	if( !realpath( fname2, buf2 ) ) {
		fprintf( stderr, "Can't fine file: %s\n", fname2 );
		exit( EXIT_FAILURE );
	}

	if( !strcmp( buf1, buf2 ) ) {
		return true;
	}
	return false;
}

bool get_fname( char * fname, int lim ) 
{
	for(;;) {
		char c;
		if( (c = getchar()) == EOF ) {
			return false;
		}
		ungetc( c, stdin );
		if( get_line( fname, lim ) == 0 ) {
			printf( "Try again (Ctrl-D to exit: " );
			continue;
		}
		else 
			return true;
	}
}

bool close_file( FILE ** fp )
{
	if( !fp || !*fp || fclose( *fp ) == EOF ) {
		return false;
	}
	*fp = NULL;
	return true;
}
