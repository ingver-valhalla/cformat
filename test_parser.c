// test_parser.c

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "in_out.h"
#include "token.h"
#include "parser.h"


const char * str_check = "int " "\'sjkdfhjkh\'  ""\"string\" "" kjhsdf ""\"STROKA\"";
const char * token_str[] = 
{
    "NOTOKEN",

    "PREPROC",
    "MUL_COMMENT",
    "COMMENT",

    "IF_KW",
    "ELSE_KW",
    "FOR_KW",
    "WHILE_KW",
    "DO_KW",
    "SWITCH_KW",
    "CASE_KW",
    "DEFAULT_KW",

    "IDENT",
    "NUM_CONST",
    "CHR_LIT",
    "STR_LIT",

    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",

    "ASSIGN_OP",
    "OP",
    "ELLIPSIS",
    "STRUCT_SEP",

    "COMMA",
    "SEMICOLON",
    "COLON",
    "QUESTION",

    "EOL_TOK",
    "EOF_TOK",

    "LAST_TOKEN_TYPE"
};


void fail( char * msg )
{
	puts( msg );
	exit( EXIT_FAILURE );
}

int alloc_buffer( char * fname, FileBuf * buf )
{
	struct stat fstat;
	if( stat( fname, & fstat ) ) {
		printf( "Unable to get stat of %s\n", fname );
		return 0;
	}
	if( !alloc_fbuf( buf, fstat.st_size ) ) {
		return 0;
	}
	return 1;
}

int how_much_eols( Token * tk )
{
	char *p = tk->start;
	int eols = 0;
	while( p != tk->end ) {
		if( *p == '\n' ) 
			++eols;
		++p;
	}
	return eols;
}

void print_token( Token * tk, int cur_line, FILE * out )
{
	printf( "Line: %3d @ Type: %-11s @ ", cur_line, token_str[tk->type] ); 
	const char * p = tk->start;
	while( p != tk->end ) {
		if( *p == ' ' )
			putchar('@');
		else 
			putchar( *p );
		++p;
	}
	printf("\n");
}

int main()
{
	FILE * in;
	FILE * out;
	char * in_fname = "example.c";
	char * out_fname = "result.c";
	if( !open_file( &in, in_fname, "r" ) ) {
		fail( "Couldn't open input" );
	}
	puts( "Opened input" );
	FileBuf buf = new_fbuf();
	if( !alloc_buffer( in_fname, &buf) ) {
		fail( "Couldn't alloc buffer" );
	}
	puts( "Allocated buffer" );
	if( !read_to_buf( in, &buf ) ) {
		fail( "Couldn't read to buffer" );
	}

	
	if( !open_file( &out, out_fname, "w" ) ) {
		fail( "Couldn't open output" );
	}

	Parser parser = new_parser();
	puts( "Created parser" );

	puts( "Parsing..." );
	if( !parse( &parser, &buf, out ) )
		printf( "Error occured while parsing\n"
		        "Current line: %d\n",
		        parser.cur_line );
	if( !free_fbuf( &buf ) )
		fail( "Couldn't free buffer" );
	puts( "Freed buffer" );
	if( !free_parser( &parser ) )
		fail( "Couldn't free parser" );
	puts( "Freed parser" );

	close_file( &in );
	close_file( &out );
	
	puts( "Exiting" );
	return 0;
}
