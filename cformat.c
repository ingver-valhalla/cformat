// cformat.c

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "in_out.h"

#define MAX_LINE 100

void print_help( void )
{
	printf( "Usage: cformat [<input> <output>]\n" );
}

bool alloc_buffer( char * fname, char ** buf, size_t * size )
{
	struct stat fstat;
	if( stat( fname, &fstat ) ) {
		printf( "Unable to get stat of %s\n", fname );
		return false;
	}
	*size = fstat.st_size;
	*buf = (char *) calloc( *size + 1, 1 );
	if( !*buf ) {
		printf( "Can't allocate memory for %s\n", fname );
		return false;
	}
	(*buf)[*size] = EOF;
	return true;
}

bool read_to_buf( FILE * fp, char * buf, size_t size )
{
	if( !fp || !buf || size != fread( buf, 1, size, fp ) ) {
		return false;
	}
	return true;
}

void print_buf( char * buf, size_t size ) 
{
	char * end = buf + size;
	while( buf != end ) {
		putchar( *buf++ );
	}
}

bool buf_to_file( FILE * fp, char * buf, size_t size )
{
	if( !fp || !buf || size != fwrite( buf, 1, size, fp ) ) {
		return false;
	}
	return true;
}

void fail( char * msg )
{
	puts( msg );
	exit( EXIT_FAILURE );
}

int main( int argc, char * argv[] )
{
	FILE  *in_file = NULL;
	FILE  *out_file = NULL;
	FILE **second_file = NULL; /* equals (*in_file) if one file passed */

	char in_fname[MAX_LINE];
	char out_fname[MAX_LINE];
	char * second_fname; /* equals in_fname if one file passed */

	char * file_buffer = NULL;
	size_t buf_size = 0;
	
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
		print_help();
		exit( EXIT_SUCCESS );
	}
	else {
/**for debug****/
		printf( "Arguments passed\n" ); 
/***************/
		strncmp( in_fname, argv[1], MAX_LINE );
		in_fname[MAX_LINE - 1] = '\0';
		strncmp( out_fname, argv[2], MAX_LINE );
		out_fname[MAX_LINE - 1] = '\0';
	}


	if( same_file( in_fname, out_fname ) ) {
/**for debug*******/
		printf( "It's the same file\n" );
/******************/
		second_file = &in_file;
		second_fname = in_fname;
	}
	else {
/**for debug*******/
		printf( "These are different\n" );
/******************/
		second_file = &out_file;
		second_fname = out_fname;
	}
	/* Reading file to buffer */
	if( !open_file( &in_file, in_fname, "r" ) ) {
		fail("Failed open file in \'r\' mode");
	}

	if( !alloc_buffer( in_fname, &file_buffer, &buf_size ) ) {
		fail( "Can't allocate memory for input file" );
	}
	printf( "Allocated %lu bytes for %s\n", buf_size, in_fname );
	
	if( !read_to_buf( in_file, file_buffer, buf_size ) ) {
		fail( "Can't read input file" );
	}
	print_buf( file_buffer, buf_size );

	if( !close_file( &in_file ) ) {
		fail( "Error occured while closing file" );
	}
	
	if( !open_file( second_file, second_fname, "w" ) ) {
		fail("Failed open file in \'w\' mode");
	}
/**for debug******/
	if( !buf_to_file( *second_file, file_buffer, buf_size ) ) {
		fail( "Error occured while writing buffer to file" );
	}
/*****************/
	/* Ended processing a file. Returning */
	return 0;
}
