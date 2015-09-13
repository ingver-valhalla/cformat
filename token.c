// token.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

Token new_tok()
{
	Token tk;
	tk.type  = NOTOKEN;
	tk.start = NULL;
	tk.end   = NULL;

	return tk;
}

static int first_word( const char * src, const char * word )
{
	const char * end = src + strlen( word );
	return strstr( src, word ) == src 
	       && !isalnum( *end )
	       && *end != '_';
}

Token get_token( char * ptr )
{
	Token tk = new_tok();

	while( isspace( *ptr ) )
		++ptr;

	/* preprocessor directive */
	if( *ptr == '#' ) {
		tk.type  = PREPROC;
		tk.start = ptr;
		
		++ptr;
		while( 1 ) {
			if( *ptr == '\0' || *ptr == EOF
			    || (*ptr == '\n' && *(ptr-1) != '\\') )
			{
				tk.end = ptr;
				return tk;
			}
			++ptr;
		}
	}
	/* multi-line commentary */
	else if( *ptr == '/' && ptr[1] == '*' ) {
		tk.type  = MUL_COMMENT;
		tk.start = ptr;

		ptr += 2;
		while( 1 ) {
			/* unexpected EOS or EOF */
			if( *ptr == '\0' || *ptr == EOF ) {
				tk = new_tok();
				return tk;
			}
			if( *ptr == '/' && *(ptr-1) == '*' ) {
				tk.end = ptr + 1;
				return tk;
			}
			++ptr;
		}
	}
	/* commentary */
	else if( *ptr == '/' && *(ptr+1) == '/' ) {
		tk.type  = COMMENT;
		tk.start = ptr;
		
		ptr += 2;
		while( 1 ) {
			if( *ptr == '\n' && *(ptr-1) != '\\' ) {
				tk.end = ptr;
				return tk;
			}
			++ptr;
		}
	}
	/* if keyword */
	else if( first_word( ptr, "if" ) ) {
		tk.type  = IF_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "if" );
	}
	/* else keyword */
	else if( first_word( ptr, "else" ) ) {
		tk.type  = ELSE_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "else" );
	}
	/* for keyword */
	else if( first_word( ptr, "for" ) ) {
		tk.type  = FOR_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "for" );
	}
	/* while keyword */
	else if( first_word( ptr, "while" ) ) {
		tk.type  = WHILE_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "while" );
	}
	/* do keyword */
	else if( first_word( ptr, "do" ) ) {
		tk.type  = DO_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "do" );
	}
	/* switch keyword */
	else if( first_word( ptr, "switch" ) ) {
		tk.type  = SWITCH_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "switch" );
	}
	/* case keyword */
	else if( first_word( ptr, "case" ) ) {
		tk.type  = CASE_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "case" );
	}
	/* default keyword*/
	else if( first_word( ptr, "default" ) ) {
		tk.type  = DEFAULT_KW;
		tk.start = ptr;
		tk.end   = ptr + strlen( "default" );
	}
	/* identifier */
	else if( isalpha( *ptr ) || *ptr == '_' ) {
		tk.type  = IDENT;
		tk.start = ptr;

		++ptr;
		while( isalnum( *ptr ) || *ptr == '_' )
			++ptr;
		
		tk.end = ptr;
	}
	/* numeric constant */
	else if( isdigit( *ptr ) ) {
		tk.type = NUM_CONST;
		tk.start = ptr;

		++ptr;
		/* Don't care much about syntax correctness */
		while( isxdigit( *ptr ) || *ptr == '.' || *ptr == '+' || *ptr == '-' )
			++ptr;
		
		tk.end = ptr;
	}
	/* char literal */
	else if( *ptr == '\'' ) {
		tk.type  = CHR_LIT;
		tk.start = ptr;

		/* if found '\'' or '\\' literal */
		if( ptr[1] == '\\' && (ptr[2] == '\'' || ptr[2] == '\\') 
		    && ptr[3] == '\'' ) {
			tk.end = ptr + 4;
			return tk;
		}
		++ptr;
		while( 1 ) {
			/* unexpeted EOL or EOS or EOF */
			if( *ptr == '\n' || *ptr == '\0' || *ptr == EOF ) {
				tk = new_tok();
				return tk;
			}
			if( *ptr == '\'' ) {
				tk.end = ptr + 1;
				return tk;
			}
			++ptr;
		}
	}
	/* string literal */
	else if( *ptr == '\"' ) {
		tk.type = STR_LIT;
		tk.start = ptr;

		++ptr;
		while( 1 ) {
			/* unexpected EOF or EOS */
			if( *ptr == EOF || *ptr == '\0' ) {
				tk = new_tok();
				return tk;
			}
			/* unexpected EOL */
			if( *ptr == '\n' && *(ptr-1) != '\\' ) { 
				tk = new_tok();
				return tk;
			}

			if( *ptr == '\"' && *(ptr-1) != '\\' ) {
				tk.end = ptr + 1;
				return tk;
			}
			++ptr;
		}
	}
	/* left paren */
	else if( *ptr == '(' ) {
		tk.type  = LPAREN;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* right paren */
	else if( *ptr == ')' ) {
		tk.type  = RPAREN;
		tk.start = ptr;
		tk.end   = ptr + 1;
	} 
	/* left bracket */
	else if( *ptr == '[' ) {
		tk.type  = LBRACKET;
		tk.start = ptr;
		tk.end   = ptr + 1;
	} 
	/* right bracket */
	else if( *ptr == ']' ) {
		tk.type  = RBRACKET;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* left brace */
	else if( *ptr == '{' ) {
		tk.type  = LBRACE;
		tk.start = ptr;
		tk.end   = ptr + 1;
	} 
	/* right brace */
	else if( *ptr == '}' ) {
		tk.type  = RBRACE;
		tk.start = ptr;
		tk.end   = ptr + 1;
	} 
	/* assignment operators */
	else if( (*ptr == '=' && ptr[1] != '=' )
	         || (*ptr == '+' && ptr[1] == '=' )
	         || (*ptr == '-' && ptr[1] == '=' ) 
	         || (*ptr == '*' && ptr[1] == '=' )
	         || (*ptr == '/' && ptr[1] == '=' )
	         || (*ptr == '%' && ptr[1] == '=' )
	         || (*ptr == '&' && ptr[1] == '=' )
	         || (*ptr == '^' && ptr[1] == '=' )
	         || (*ptr == '|' && ptr[1] == '=' )
	         || (*ptr == '<' && ptr[1] == '<' && ptr[2] == '=' )
	         || (*ptr == '>' && ptr[1] == '>' && ptr[2] == '=' ) )
	{
		tk.type  = ASSIGN_OP;
		tk.start = ptr;
		tk.end   = strchr( ptr, '=' ) + 1;
	} 
	/* two-symbol operators */
	else if( (*ptr == '+' && ptr[1] == '+')
	         || (*ptr == '-' && ptr[1] == '-')
	         || (*ptr == '<' && ptr[1] == '<')
	         || (*ptr == '>' && ptr[1] == '>')
	         || (*ptr == '&' && ptr[1] == '&')
	         || (*ptr == '|' && ptr[1] == '|')
	         || (*ptr == '<' && ptr[1] == '=')
	         || (*ptr == '>' && ptr[1] == '=') 
	         || (*ptr == '=' && ptr[1] == '=')
	         || (*ptr == '!' && ptr[1] == '=') )
	{
		tk.type  = OP;
		tk.start = ptr;
		tk.end   = ptr + 2;
	}
	/* one-symbol operators */
	else if( *ptr == '*' || *ptr == '/' 
	         || *ptr == '+' || (*ptr == '-' && ptr[1] != '>' )
	         || *ptr == '~' || *ptr == '!' 
	         || *ptr == '&' || *ptr == '|' 
	         || *ptr == '%' || *ptr == '^'
	         || *ptr == '<' || *ptr == '>' )
	{
		tk.type  = OP;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* ellipsis */
	else if( *ptr == '.' && ptr[1] == '.' && ptr[2] == '.' ) {
		tk.type  = ELLIPSIS;
		tk.start = ptr;
		tk.end   = ptr + 3;
	}
	/* struct separator */
	else if( *ptr == '.' || (*ptr == '-' && ptr[1] == '>') ) {
	         tk.type  = STRUCT_SEP;
	         tk.start = ptr;
	         tk.end   = ptr + 1 + (*ptr == '-');
	}
	/* comma */
	else if( *ptr == ',' ) {
		tk.type  = COMMA;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* semicolon */
	else if( *ptr == ';' ) {
		tk.type  = SEMICOLON;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* colon */
	else if( *ptr == ':' ) {
		tk.type  = COLON;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* question mark */
	else if( *ptr == '?' ) {
		tk.type  = QUESTION;
		tk.start = ptr;
		tk.end   = ptr + 1;
	}
	/* caught end of file */
	else if( *ptr == EOF ) {
		tk.type = EOF_TOK;
		tk.start = ptr;
		tk.end = ptr + 1;
	}

	return tk;
}
