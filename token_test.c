#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "in_out.h"
#include "buf.h"
#include "token.h"


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

    "EOF_TOK",

    "LAST_TOKEN_TYPE"
};


void fail( char * msg )
{
	puts( msg );
	exit( EXIT_FAILURE );
}

bool alloc_buffer( char * fname, FileBuf * buf )
{
	struct stat fstat;
	if( stat( fname, & fstat ) ) {
		printf( "Unable to get stat of %s\n", fname );
		return false;
	}
	if( !alloc_fbuf( buf, fstat.st_size ) ) {
		return false;
	}
	return true;
}


void print_token( Token * tk, FILE * out )
{
	printf( "Type: %-11s @ ", token_str[tk->type] ); 
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
	FILE *fp = NULL;
	char * fname;
	fname = "example.c";
	/*fname = (char *)malloc( 100 );*/
	/*printf( "Input file: " );*/
	/*get_line( fname, 100 );*/
	
	if( !open_file( &fp, fname, "r" ) ) {
		fail( "Couldn't open input" );
	}
	puts( "Opened input" );
	FileBuf buf = new_fbuf();
	if( !alloc_buffer( fname, &buf) ) {
		fail( "Couldn't alloc buffer" );
	}
	puts( "Allocated buffer" );
	if( !read_to_buf( fp, &buf ) ) {
		fail( "Couldn't read to buffer" );
	}
	puts( "Read buffer" );
	
	Token tk = new_tok();
	puts( "Getting tokens" );
	while( 1 ) {
		tk = get_token( buf.head );
		if( tk.type == NOTOKEN ) {
			printf( "Got no token\n" );
			break;
		}
		buf.head = tk.end;
		print_token( &tk, stdout );
	}
	
	return 0;
}
