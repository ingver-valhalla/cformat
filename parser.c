// parser.c 

#include <stdlib.h>
#include "parser.h"

static void reset_parser_state( Parser * p )
{
	p->state.same_line = 0;
	p->state.cur_line    = 1;
	p->state.brace_depth = 0;
	p->state.prev_tk     = NOTOKEN;
}

Parser new_parser()
{
	Parser p;
	p.buf = NULL;

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
	if( parser->state.prev_tk == ELSE_KW ) {
		putc( ' ', out );	
	}
	else {
		putc( '\n', out );
	}       
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_else_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_for_kw( Token * tk, Parser * parser, FILE * out ) 
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
	    return 0;
	}
	return 1;	    
}
static int handle_token_while_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_do_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_switch( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_case_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_default_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_ident( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_num_const( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_chr_lit( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_str_lit( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_lparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_rparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_lbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_rbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_lbracket( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_rbracket( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_assign_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_ellipsis( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_struct_sep( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_comma( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_semicolon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_colon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_question( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_eol( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
	return 1;
}

static int handle_token_eof( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( '\n', out );
	if( !push_token( tk, out ) ) {
		return 0;
	}
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
