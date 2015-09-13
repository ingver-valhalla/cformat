// cformat.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "in_out.h"
#include "buf.h"

#define DEBUG_ON

#define MAX_LINE 100

void print_help( char ** argv )
{
	fprintf( stderr, "Usage: %s [<input> <output>]\n", argv[0] );
}

void fail( const char * msg )
{
	fputs( msg, stderr );
	fprintf( stderr, "\n" );
	exit( EXIT_FAILURE );
}

int alloc_buffer( char * fname, FileBuf * buf )
{
	struct stat fstat;
	if( stat( fname, & fstat ) ) {
		fprintf( stderr, "Unable to get stat of %s\n", fname );
		return 0;
	}
	if( !alloc_fbuf( buf, fstat.st_size ) ) {
		return 0;
	}
	return 1;
}


int main( int argc, char * argv[] )
{
	FILE  *  in_file = NULL;
	FILE  *  out_file = NULL;
	FILE  ** second_file = NULL; /* equals (*in_file) if one file passed */

	char   in_fname[MAX_LINE];
	char   out_fname[MAX_LINE];
	char * second_fname; /* equals in_fname if one file passed */

	FileBuf file_buffer = new_fbuf();
	
	if( argc == 1 ) {
		/* Program called without arguments. 
		* Get file names from user */
		printf( "Enter input file name (Ctrl-D to exit): "); 
		if( !get_fname( in_fname, MAX_LINE ) ) {
			/* Exiting on Ctrl-D */
			exit( EXIT_SUCCESS );
		}
		printf( "Enter output file name (Ctrl-D to exit): " );
		if( !get_fname( out_fname, MAX_LINE ) ) {
			/* Exiting on Ctrl-D */
		    exit( EXIT_SUCCESS );
		}
	}
	else if( argc != 3 ) {
		print_help( argv );
		exit( EXIT_SUCCESS );
	}
	else {
#ifdef DEBUG_ON
		printf( "Arguments passed\n" ); 
#endif
		strncpy( in_fname, argv[1], MAX_LINE );
		in_fname[MAX_LINE - 1] = '\0';
		strncpy( out_fname, argv[2], MAX_LINE );
		out_fname[MAX_LINE - 1] = '\0';
	}


	if( same_file( in_fname, out_fname ) ) {
#ifdef DEBUG_ON
		printf( "It's the same file\n" );
#endif
		second_file = &in_file;
		second_fname = in_fname;
	}
	else {
#ifdef DEBUG_ON
		printf( "These are different\n" );
#endif
		second_file = &out_file;
		second_fname = out_fname;
	}
	/* Reading file to buffer */
	if( !open_file( &in_file, in_fname, "r" ) ) {
		fail("Failed open file in \'r\' mode");
	}

	if( !alloc_buffer( in_fname, &file_buffer ) ) {
		fail( "Can't allocate memory for input file" );
	}
#ifndef DEBUG_ON
	printf( "Allocated %lu bytes for %s\n", file_buffer.size, in_fname );
#endif
	
	if( !read_to_buf( in_file, &file_buffer ) ) {
		fail( "Can't read input file" );
	}
	buf_to_file( stdout, &file_buffer );

	if( !close_file( &in_file ) ) {
		fail( "Error occured while closing file" );
	}
	
	if( !open_file( second_file, second_fname, "w" ) ) {
		fail("Failed open file in \'w\' mode");
	}
#ifdef DEBUG_ON
	if( !buf_to_file( *second_file, &file_buffer ) ) {
		fail( "Error occured while writing buffer to file" );
	}
#endif
	/* Ended processing a file. Returning */
	return 0;
}
