// parser.c 

#include <stdlib.h>
#include "parser.h"

static void reset_parser_state( Parser * p )
{
	p->state.empty_line   = 1;
	p->state.un_op        = 0;
	p->state.cur_line     = 1;
	p->state.brace_depth  = 0;
	p->state.prev_tk.type = NOTOKEN;
}

Parser new_parser()
{
	Parser p;
	p.buf = NULL;

	reset_parser_state( &p );

	return p;
}

static int push_token( Token * tk, FILE * out )
{
	char *p = tk->start;
	while( p != tk->end ) {
		if( putc( *p, out ) == EOF )
			return 0;
		++p;
	}
	return 1;
}

static int handle_token_mcomment( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;

	if( !parser->state.empty_line ) {
		putc( '\t', out );
	}
	if( !push_token( tk, out ) ) 
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = MUL_COMMENT;
	return 1;
}

static int handle_token_comment( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !parser->state.empty_line ) { 
		putc( '\t', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = COMMENT;
	return 1;
}


static int handle_token_if_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type == ELSE_KW ) {
		putc( ' ', out );	
	}
	else if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}       
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = IF_KW;
	return 1;
}

static int handle_token_else_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = ELSE_KW;
	return 1;
}

static int handle_token_for_kw( Token * tk, Parser * parser, FILE * out ) 
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
	    return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = FOR_KW;
	return 1;	    
}
static int handle_token_while_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = WHILE_KW;
	return 1;
}

static int handle_token_do_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = DO_KW;
	return 1;
}

static int handle_token_switch_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = SWITCH_KW;
	return 1;
}

static int handle_token_case_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = CASE_KW;
	return 1;
}

static int handle_token_default_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = DEFAULT_KW;
	return 1;
}

static int handle_token_ident( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && parser->state.prev_tk.type != STRUCT_SEP 
	    && parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK
	    && !parser->state.un_op )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = IDENT;
	parser->state.un_op = 0;
	return 1;
}

static int handle_token_num_const( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK
	    && !parser->state.un_op )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = NUM_CONST;
	parser->state.un_op = 0;
	return 1;
}

static int handle_token_chr_lit( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK
	    && !parser->state.un_op )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = CHR_LIT;
	parser->state.un_op = 0;
	return 1;
}

static int handle_token_str_lit( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK
	    && !parser->state.un_op )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = STR_LIT;
	parser->state.un_op = 0;
	return 1;
}

static int handle_token_lparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = LPAREN;
	return 1;
}

static int handle_token_rparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/* putting space before token if needed */
	/*if( parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != RPAREN
	    && parser->state.prev_tk.type != RBRACKET
	    && parser->state.prev_tk.type != EOL_TOK )
	{
		putc( ' ', out );
	}*/
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = RPAREN;
	return 1;
}

static int handle_token_lbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK)
	{
		putc( ' ', out );
	}
	if( parser->state.prev_tk.type != EOL_TOK 
	    && parser->state.prev_tk.type != ASSIGN_OP )
	{
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = LBRACE;
	return 1;
}

static int handle_token_rbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != SEMICOLON
	    && parser->state.prev_tk.type != EOL_TOK )
	{
		putc( ' ', out );
	}
	if( parser->state.prev_tk.type != EOL_TOK ) {
		putc( '\n', out );
		parser->state.empty_line = 1;
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = RBRACE;
	return 1;
}

static int handle_token_lbracket( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = LBRACKET;
	return 1;
}

static int handle_token_rbracket( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = LBRACKET;
	return 1;
}

static int handle_token_assign_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( ' ', out );
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = ASSIGN_OP;
	return 1;
}

static int handle_token_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !parser->state.empty_line
	    && parser->state.prev_tk.type != LPAREN
	    && parser->state.prev_tk.type != LBRACKET
	    && !is_incr_or_decr( tk ) )
	{
		putc( ' ', out );
	}
	/* find out if this is an unary op */
	if( is_unary_op( tk )
	    && parser->state.prev_tk.type != IDENT
	    && parser->state.prev_tk.type != NUM_CONST
	    && parser->state.prev_tk.type != CHR_LIT
	    && parser->state.prev_tk.type != STR_LIT )
	{
		parser->state.un_op = 1;
	}
	else {
		parser->state.un_op = 0;
	}

	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = OP;
	return 1;
}

static int handle_token_ellipsis( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( ' ', out );
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = ELLIPSIS;
	return 1;
}

static int handle_token_struct_sep( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = STRUCT_SEP;
	return 1;
}

static int handle_token_comma( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = COMMA;
	return 1;
}

static int handle_token_semicolon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = SEMICOLON;
	return 1;
}

static int handle_token_colon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->state.prev_tk.type != EOL_TOK )
		putc( ' ', out );
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = COLON;
	return 1;
}

static int handle_token_question( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	putc( ' ', out );
	if( !push_token( tk, out ) )
		return 0;
	parser->state.empty_line = 0;
	parser->state.prev_tk.type = QUESTION;
	return 1;
}

static int handle_token_eol( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) ) {
		return 0;
	}
	parser->state.empty_line = 1;
	parser->state.prev_tk.type = EOL_TOK;
	return 1;
}

static int handle_token_eof( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	parser->state.prev_tk.type = EOF_TOK;
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
	else if( tk->type == ELSE_KW ) {
		return handle_token_else_kw( tk, parser, out );
	}
	else if( tk->type == FOR_KW ) {
		return handle_token_for_kw( tk, parser, out );
	}
	else if( tk->type == WHILE_KW ) {
		return handle_token_while_kw( tk, parser, out );
	}
	else if( tk->type == DO_KW ) {
		return handle_token_do_kw( tk, parser, out );
	}
	else if( tk->type == SWITCH_KW ) {
		return handle_token_switch_kw( tk, parser, out );
	}
	else if( tk->type == CASE_KW ) {
		return handle_token_case_kw( tk, parser, out );
	}
	else if( tk->type == DEFAULT_KW ) {
		return handle_token_default_kw( tk, parser, out );
	}
	else if( tk->type == IDENT ) {
		return handle_token_ident( tk, parser, out );
	}
	else if( tk->type == NUM_CONST ) {
		return handle_token_num_const( tk, parser, out );
	}
	else if( tk->type == CHR_LIT ) {
		return handle_token_chr_lit( tk, parser, out );
	}
	else if( tk->type == STR_LIT ) {
		return handle_token_str_lit( tk, parser, out );
	}
	else if( tk->type == LPAREN ) {
		return handle_token_lparen( tk, parser, out );
	}
	else if( tk->type == RPAREN ) {
		return handle_token_rparen( tk, parser, out );
	}
	else if( tk->type == LBRACE ) {
		return handle_token_lbrace( tk, parser, out );
	}
	else if( tk->type == RBRACE ) {
		return handle_token_rbrace( tk, parser, out );
	}
	else if( tk->type == LBRACKET ) {
		return handle_token_lbracket( tk, parser, out );
	}
	else if( tk->type == RBRACKET ) {
		return handle_token_rbracket( tk, parser, out );
	}
	else if( tk->type == ASSIGN_OP ) {
		return handle_token_assign_op( tk, parser, out );
	}
	else if( tk->type == OP ) {
		return handle_token_op( tk, parser, out );
	}
	else if( tk->type == ELLIPSIS ) {
		return handle_token_ellipsis( tk, parser, out );
	}
	else if( tk->type == STRUCT_SEP ) {
		return handle_token_struct_sep( tk, parser, out );
	}
	else if( tk->type == COMMA ) {
		return handle_token_comma( tk, parser, out );
	}
	else if( tk->type == SEMICOLON ) {
		return handle_token_semicolon( tk, parser, out );
	}
	else if( tk->type == COLON ) {
		return handle_token_colon( tk, parser, out );
	}
	else if( tk->type == QUESTION ) {
		return handle_token_question( tk, parser, out );
	}
	else if( tk->type == EOL_TOK ) {
		return handle_token_eol( tk, parser, out );
	}
	else if( tk->type == EOF_TOK ) {
		return handle_token_eof( tk, parser, out );
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
		
		if( !handle_token( &cur_tk, parser, out ) )
			return 0;
		parser->buf->head = cur_tk.end;
	}
	return 1;
}
