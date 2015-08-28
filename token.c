// token.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "token.h"

Token new_tok()
{
	Token tk;
	tk.type  = NOTOKEN;
	tk.start = NULL;
	tk.end   = NULL;

	return tk;
}
Token get_token( const char * ptr )
{
	Token tk = new_tok();

	while( isspace( *ptr ) )
		++ptr;
	
	/* identifier */
	if( isalpha( *ptr ) || *ptr == '_' ) {
		tk.type  = IDENT;
		tk.start = ptr;

		++ptr;
		while( isalnum( *ptr ) || *ptr == '_' ) {
			++ptr;
		}
		tk.end = ptr;
		return tk;
	}
	/* char literal */
	else if( *ptr == '\'' ) {
		tk.type  = CHAR;
		tk.start = ptr;
		++ptr;
		while( *ptr != '\'' ) {
			/* unexpeted EOL or EOS or EOF */
			if( *ptr == '\n' || *ptr == '\0' || *ptr == EOF ) {
				tk = new_tok();
				return tk;
			}
			++ptr;
		}
		tk.end = ptr + 1;
		return tk;
	}
	/* string literal */
	else if( *ptr == '\"' ) {
		tk.type = STRING;
		tk.start = ptr;
		++ptr;
		while( 1 ) {
			/* unexpected EOF or EOS */
			if( *ptr == EOF || *ptr == '\0' ) {
				tk = new_tok();
				return tk;
			}
			/* unexpected EOL */
			if( *ptr == '\n' && *ptr != '\\' ) { 
				tk = new_tok();
				return tk;
			}
			/* reached end of string  literal */
			if( *ptr == '\"' && *(ptr-1) != '\\' ) {
				tk.end = ptr + 1;
				return tk;
			}
			++ptr;
		}
	}
	return tk;
}
