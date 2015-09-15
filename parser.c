// parser.c 

#include <stdlib.h>
#include "parser.h"

static void reset_parser_state( Parser * p )
{
	p->state.same_line = 0;
}

Parser new_parser()
{
	Parser p;
	p.buf         = NULL;
	p.cur_line    = 1;
	p.brace_depth = 0;
	p.prev_tk     = NOTOKEN;

	reset_parser_state( &p );

	return p;
}

static int handle_token_mcomment( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;

	if( parser->state.same_line ) {
		putc( '\t', out );
	}
	if( !push_token( tk, out ) ) 
		return 0;
	return 1;
}

static int handle_token_comment( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.same_line ) { 
		putc( '\t', out );
	}
	if( !push_token( tk, out ) )
		return 0;

	return 1;
}


static int handle_token_if_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	return 1;
}

static int handle_token( Token * tk, Parser * parser, FILE * out)
{
	/* PREPROC */
	if( tk->type == PREPROC ) {
		if( !push_token( tk, out ) ) {
			return 0;
		}
		return 1;
	}
	/* MUL_COMMENT */
	else if( tk->type == MUL_COMMENT ) {
		return handle_token_mcomment( tk, parser, out );
	}
	else if( tk->type == COMMENT ) {
		return handle_token_comment( tk, parser, out );	
	}
	else if( tk->type == IF_KW ) {
		return handle_token_if_kw( tk, parser, out );
	}
	else
		return 0;
	return 1;
}

int parse( Parser * parser, FILE * out )
{
	if( !parser || !out )
		return 0;

	Token cur_tk = new_tok();

	for(;;) {
		cur_tk = get_token( parser->buf->head );
		if( cur_tk.type == NOTOKEN ) {
			break;
		}
		
		handle_token( &cur_tk, parser, out );
		parser->buf->head = cur_tk.end;
	}
	return 1;
}
