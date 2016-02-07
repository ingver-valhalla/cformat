// parser.c 

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "parser.h"

#define DEBUG_ON

#ifdef DEBUG_ON
	static void check_parser( Parser * parser );
#endif

Parser new_parser()
{
	Parser p;

	p.prev_tk            = new_tok();
	/*p.prev_nonwhite_tk   = new_tok();*/
	p.empty_line         = 1;
	p.un_op              = 0;
	p.in_branch          = 0;
	p.parens_closed      = 1;
	/*p.prev_closing_paren = 0;*/
	p.cur_line           = 1;
	p.paren_depth        = 0;
	p.indent             = 0;
	p.brace_indent       = malloc( 10*sizeof(int) );
	p.brace_indent[0]    = 0;
	p.stack_size         = 10;
	p.last_brace_indent  = 0;

	return p;
}

int free_parser( Parser * p )
{
	if( !p )
		return 0;

	if( p->brace_indent ) {
		free( p->brace_indent );
	}
	memset( p, 0, sizeof( *p ) );

	return 1;
}

static int enlarge_buffer( int ** buf, int * cur_size )
{
	if( !buf || !*buf || *cur_size < 0 )
		return 0;
	*buf = realloc( *buf, (*cur_size+10)*sizeof(int) );
	if( !buf )
		return 0;
	*cur_size += 10;
	return 1;
}
static int shrink_buffer( int ** buf, int * cur_size )
{
	if( !buf || !*buf || *cur_size < 10 )
		return 0;
	*buf = realloc( *buf, (*cur_size-10)*sizeof(int) );
	if( !buf )
		return 0;
	*cur_size -= 10;
	return 1;
}

static int how_much_eols( Token * tk )
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

static int push_indent( int indent, FILE * out )
{
	if( !out )
		return 0;
	while( indent-- > 0 ) {
		if( putc( '\t', out ) == EOF )
			return 0;
	}
	return 1;
}

static int handle_token_comment( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !parser->empty_line ) { 
		putc( '\t', out );
	}
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	return 1;
}


static int handle_token_cond( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( tk->type == IF_KW && parser->prev_tk.type == ELSE_KW ) {
		putc( ' ', out );	
	}
	/*else if( parser->prev_tk.type != EOL_TOK ) {*/
	else if( !parser->empty_line ) {
		putc( '\n', out );
		parser->empty_line = 1;
	}
	if( parser->in_branch
		/*&& parser->prev_nonwhite_tk.type != ELSE_KW) */
		&& parser->prev_tk.type != ELSE_KW )
	{
		++parser->indent;
	}
	else {
		parser->in_branch = 1;
	}
	if( tk->type != ELSE_KW )
		parser->parens_closed = 0;
	else {
		parser->parens_closed = 1;
	}
	
	if( parser->prev_tk.type != ELSE_KW
	    && !push_indent( parser->indent, out ) )
		return 0;

	if( !push_token( tk, out ) )
		return 0;

	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_case_kw( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	//if( parser->prev_tk.type != EOL_TOK ) {
	if( !parser->empty_line ) {
		putc( '\n', out );
		parser->empty_line = 1;
	}
	parser->indent = parser->brace_indent[parser->last_brace_indent];
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	++parser->indent;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_ident( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
#ifdef DEBUG_ON
	/*if( parser->un_op )
		 putc( '@', out );
	else
		 putc( '#', out );*/
#endif
	if( !parser->in_branch
	    && !parser->empty_line
	    && (parser->prev_tk.type == LBRACE
	        || parser->prev_tk.type == SEMICOLON ) )
	{
		putc('\n', out );
		parser->empty_line = 1;
	}
	/* putting space before token if needed */
	else if( !parser->empty_line
	         && parser->prev_tk.type != LPAREN
	         && parser->prev_tk.type != LBRACKET
	         && parser->prev_tk.type != STRUCT_SEP
	         //&& parser->prev_tk.type != EOL_TOK
	         && !parser->un_op )
	{
		putc( ' ', out );
	} 
	if( parser->in_branch
	    && parser->parens_closed )
	{
		if( parser->prev_tk.type == RPAREN
		    && !parser->empty_line )
		{
			putc( '\n', out );
			parser->empty_line = 1;
		}
		parser->in_branch = 0;
		++parser->indent;
	}
	if( parser->empty_line
	    && !push_indent( parser->indent, out ) )
		return 0;
	if( parser->in_branch
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
		
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	parser->prev_tk.type = IDENT;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_lparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	
	// --------------------------
	// parser->parens_closed == 0
	if( !parser->parens_closed )
		++parser->paren_depth;

	if( parser->empty_line
	    && !parser->parens_closed )
	{
		if( !push_indent( parser->indent, out ) )
			return 0;
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	// --------------------------


	// --------------------------
	// parser->parens_closed == 1
	if( parser->in_branch
	    && parser->parens_closed )
	{
		if( !parser->empty_line
		    && parser->prev_tk.type == RPAREN )
		{
			putc( '\n', out );
			parser->empty_line = 1;
		}
		++parser->indent;

		if( !push_indent( parser->indent, out ) )
			return 0;
		// didn't set parser->in_branch to 0, because next if will execute
	}
	// --------------------------
	

	// ----------------------
	// parser->in_branch == 0
	if( !parser->in_branch
		/*&& parser->empty_line*/ )
	{
		if( !parser->empty_line ) {
			if( parser->prev_tk.type == LBRACE
			    || parser->prev_tk.type == RBRACE
			    || parser->prev_tk.type == SEMICOLON )
			{
				putc('\n', out );
				parser->empty_line = 1;
			}
		}
		if( parser->empty_line
		    && !push_indent( parser->indent, out ) )
			return 0;
	}
	// ----------------------
	
	
	// now resetting parser->in_branch
	if( parser->in_branch
	    && parser->parens_closed )
		parser->in_branch = 0;

	/* putting space before token if needed */
	if( !parser->empty_line
	    //&& parser->prev_tk.type != EOL_TOK
	    && parser->prev_tk.type != LPAREN
	    && parser->prev_tk.type != LBRACKET
	    && parser->prev_tk.type != SEMICOLON
	    && !parser->un_op )
	{
		putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_rparen( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;

	// --------------------------
	// parser->parens_closed == 0
	if( parser->in_branch
	    && !parser->parens_closed )
	{
		--parser->paren_depth;
	}
	/*if( parser->empty_line*/
		/*&& !parser->parens_closed*/
		/*&& !push_indent( parser->indent, out ))*/
		/*return 0;*/
	if( parser->in_branch
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	// --------------------------


	// --------------------------
	// parser->parens_closed == 1
	/*if( parser->empty_line*/
		/*&& parser->parens_closed*/
		/*&& !push_indent( parser->indent, out ) )*/
		/*return 0;*/
	// --------------------------
 

	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;

	if( parser->in_branch
	    && !parser->paren_depth )
	{
		parser->parens_closed = 1;
		/*parser->prev_closing_paren = 1;*/
	}
	if( parser->un_op )
		parser->un_op = 0;
	return 1;
}

static int handle_token_lbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	++parser->last_brace_indent;
	if( parser->last_brace_indent >= parser->stack_size
	    && !enlarge_buffer( &(parser->brace_indent),
		                &(parser->stack_size)  ) )
		return 0;	
	parser->brace_indent[parser->last_brace_indent]
		= parser->indent;
	//if( parser->prev_tk.type != EOL_TOK 
	if( !parser->empty_line 
	    && parser->prev_tk.type != ASSIGN_OP )
	{
		putc( '\n', out );
		parser->empty_line = 1;
	}

	/*if( parser->prev_tk.type != SEMICOLON*/
		/*&& parser->prev_tk.type != EOL_TOK*/
		/*&& !parser->prev_closing_paren)*/
	// insert a space after '=' in array init
	if( parser->prev_tk.type == ASSIGN_OP )
	{
		putc( ' ', out );
	}
	if( !push_indent( parser->indent, out ) )
		return 0;

	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;

	parser->in_branch = 0;
	++parser->indent;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_rbrace( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	int lbi = parser->brace_indent[parser->last_brace_indent];

	//if( parser->prev_tk.type != EOL_TOK ) {
	if( !parser->empty_line ) {
		putc( '\n', out );
		parser->empty_line = 1;
	}
	
	parser->indent = lbi;
	if( !push_indent( parser->indent, out ) )
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	
	--parser->last_brace_indent;
	if( parser->last_brace_indent > 0
	    && parser->last_brace_indent < parser->stack_size - 10
	    && !shrink_buffer( &(parser->brace_indent),
	                       &(parser->stack_size) ) )
		return 0;
	lbi = parser->brace_indent[parser->last_brace_indent];
	if( !parser->last_brace_indent )
		parser->indent = lbi;
	else
		parser->indent = lbi + 1;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_bracket( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( parser->in_branch
		/*&& parser->paren_depth*/
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_assign_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;

	if( !parser->empty_line)
		putc( ' ', out );

	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	    
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_op( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;

#ifdef DEBUG_ON
	/*if( parser->un_op )
		putc( '@', out );
	else
		putc( '#', out );
	if( parser->prev_closing_paren )
		putc( '$' , out );
	if( parser->empty_line )
		putc( '`', out );
	if( parser->in_branch )
		putc( '?', out );*/
#endif

	if( (is_incr_or_decr( tk ) 
	     && !(parser->un_op
		  || parser->prev_tk.type == LPAREN
		  || parser->prev_tk.type == RPAREN
		  || parser->prev_tk.type == LBRACKET
		  || parser->prev_tk.type == RBRACKET
		  || parser->prev_tk.type == IDENT
		  || parser->prev_tk.type == NUM_CONST
		  || parser->prev_tk.type == CHR_LIT
		  || parser->prev_tk.type == STR_LIT)
	    )
	    || (!is_incr_or_decr( tk )
		&& ((!is_unary_op( tk ) 
		     && (parser->un_op
			 || parser->prev_tk.type == RPAREN
			 || parser->prev_tk.type == RBRACKET
			 || parser->prev_tk.type == IDENT
			 || parser->prev_tk.type == NUM_CONST
			 || parser->prev_tk.type == CHR_LIT
			 || parser->prev_tk.type == STR_LIT))
		    || (is_unary_op( tk )
			&& (!parser->un_op 
			        || is_incr_or_decr( &(parser->prev_tk) ))
			&& !(parser->prev_tk.type == LPAREN
			     || parser->prev_tk.type == LBRACKET
			     || (parser->prev_tk.start[0] == '*'
			         && tk->start[0] == '*')))
		   )
	       )
	  )
	{
		if( !parser->empty_line 
		    && (!parser->in_branch
		        || (!parser->parens_closed
					/*&& !parser->prev_closing_paren*/)) )
			putc( ' ', out );
	}

	/* find out if this is an unary op */
	if( is_incr_or_decr( tk )
	    || (is_unary_op( tk )
	        && !is_incr_or_decr( &(parser->prev_tk) )
			/*&& parser->prev_nonwhite_tk.type != IDENT*/
			/*&& parser->prev_nonwhite_tk.type != NUM_CONST*/
			/*&& parser->prev_nonwhite_tk.type != CHR_LIT*/
			/*&& parser->prev_nonwhite_tk.type != STR_LIT*/
			/*&& (parser->prev_nonwhite_tk.type != RPAREN*/
				/*|| (parser->prev_nonwhite_tk.type == RPAREN*/
	        && parser->prev_tk.type != IDENT
	        && parser->prev_tk.type != NUM_CONST
	        && parser->prev_tk.type != CHR_LIT
	        && parser->prev_tk.type != STR_LIT
	        && (parser->prev_tk.type != RPAREN
	            || (parser->prev_tk.type == RPAREN
	                && parser->in_branch
					/*&& parser->prev_closing_paren*/ ))
			/*&& parser->prev_nonwhite_tk.type != RBRACKET) )*/
			&& parser->prev_tk.type != RBRACKET) )
	{
		parser->un_op = 1;
	}
	else {
		parser->un_op = 0;
	}

	if( parser->in_branch
	    && parser->parens_closed )
	{
		if( !parser->empty_line
			/*&& parser->prev_closing_paren*/ )
		{
			putc( '\n', out );
			parser->empty_line = 1;
		}
		parser->in_branch = 0;
		++parser->indent;
	}
	else if( !parser->in_branch ) {
		if( parser->prev_tk.type == SEMICOLON ) {
			putc( '\n', out );
			parser->empty_line = 1;
		}
	}

	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( parser->in_branch
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_ellipsis( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !parser->empty_line )
		putc( ' ', out );
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_sep( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( parser->in_branch
		/*&& parser->paren_depth*/
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_semicolon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( parser->empty_line
	    && !push_indent( parser->indent, out ) )
		return 0;
#ifdef DEBUG_ON
	/*if( parser->in_branch )
		putc( '?', out );*/
#endif
	if( parser->in_branch
		/*&& parser->paren_depth*/
	  )	
	{
		if( parser->parens_closed ) {
			parser->in_branch = 0;
		}
		else if( parser->empty_line ) {
			int i;
			for( i = 4; i > 0; --i )
				putc( ' ', out );
		}
	}
	if( !push_token( tk, out ) )
		return 0;
	if( !parser->in_branch 
		/*|| parser->prev_closing_paren*/ )
	{
		if( parser->last_brace_indent )
			parser->indent = parser->brace_indent[parser
				->last_brace_indent] + 1;
		else
			parser->indent = 0;
		/*if( parser->prev_closing_paren ) {*/
			/*parser->in_branch = 0;*/
			/*parser->prev_closing_paren = 0;*/
		/*}*/
	}
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;

	return 1;
}

static int handle_token_colon( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	/*if( parser->prev_tk.type != EOL_TOK )*/
		/*putc( ' ', out );*/
	if( parser->empty_line
	    && !push_indent( parser->indent, out ))
		return 0;
	if( parser->in_branch
		/*&& parser->paren_depth*/
	    && !parser->parens_closed
	    && parser->empty_line ) 
	{
		int i;
		for( i = 4; i > 0; --i )
			putc( ' ', out );
	}
	if( !push_token( tk, out ) )
		return 0;
	parser->empty_line = 0;
	if( parser->un_op )
		parser->un_op = 0;
	/*parser->prev_closing_paren = 0;*/
	return 1;
}

static int handle_token_eol( Token * tk, Parser * parser, FILE * out )
{
	if( !tk || !parser || !out )
		return 0;
	if( !push_token( tk, out ) ) {
		return 0;
	}
	putc( '@', out );
	parser->empty_line = 1;
	if( parser->un_op )
		parser->un_op = 0;
	return 1;
}

static int handle_token( Token * tk, Parser * parser, FILE * out)
{
	/* PREPROC */
	if( tk->type == PREPROC ) {
		if( !push_token( tk, out ) ) {
			return 0;
		}
		parser->empty_line = 0;
		return 1;
	}
	else if( tk->type == COMMENT
 		 || tk->type == MUL_COMMENT )
	{
		return handle_token_comment( tk, parser, out );	
	}
	else if( tk->type == IF_KW 
	         || tk->type == ELSE_KW
	         || tk->type == FOR_KW
	         || tk->type == WHILE_KW
	         || tk->type == DO_KW
	         || tk->type == SWITCH_KW )
	{
		return handle_token_cond( tk, parser, out );
	}
	else if( tk->type == CASE_KW
	         || tk->type == DEFAULT_KW )
	{
		return handle_token_case_kw( tk, parser, out );
	}
	else if( tk->type == IDENT 
	         || tk->type == NUM_CONST
	         || tk->type == CHR_LIT
	         || tk->type == STR_LIT )
	{
		return handle_token_ident( tk, parser, out );
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
	else if( tk->type == LBRACKET
	         || tk->type == RBRACKET )
	{
		return handle_token_bracket( tk, parser, out );
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
	else if( tk->type == STRUCT_SEP
	         || tk->type == COMMA )
	{
		return handle_token_sep( tk, parser, out );
	}
	else if( tk->type == SEMICOLON ) {
		return handle_token_semicolon( tk, parser, out );
	}
	else if( tk->type == COLON
	         || tk->type == QUESTION )
	{
		return handle_token_colon( tk, parser, out );
	}
	else if( tk->type == EOL_TOK ) {
		return handle_token_eol( tk, parser, out );
	}
	else
		return 0;
	return 1;
}

int parse( Parser * parser, FileBuf * buf, FILE * out )
{
	if( !parser || !out )
		return 0;

	Token cur_tk = new_tok();

	for(;;) {
		cur_tk = get_token( buf->head );
		if( cur_tk.type == NOTOKEN
		    || cur_tk.type == EOF_TOK )
		{
			break;
		}
		if( cur_tk.type == EOL_TOK ) {
			++parser->cur_line;
		}
		if( cur_tk.type == PREPROC
		    || cur_tk.type == MUL_COMMENT
		    || cur_tk.type == STR_LIT )
		{
			parser->cur_line += how_much_eols( &cur_tk );
		}
		
		if( !handle_token( &cur_tk, parser, out ) )
			return 0;
		if( cur_tk.type != EOL_TOK ) {
			parser->prev_tk = cur_tk;
			/*parser->prev_nonwhite_tk = cur_tk;*/
		}
		buf->head = cur_tk.end;
#ifdef DEBUG_ON
		check_parser( parser );
#endif // DEBUG_ON
	}
	return 1;
}

#ifdef DEBUG_ON
/*#define LINE_TO_STOP 229 */
/*#define TOK_TO_CATCH PREPROC */
static void check_parser( Parser * parser )
{
	if( parser->paren_depth < 0
	    || parser->indent < 0
	#if defined(LINE_TO_STOP) && defined(TOK_TO_CATCH) 
	    || (parser->cur_line == LINE_TO_STOP
	        && parser->prev_tk.type == TOK_TO_CATCH) 
	#endif
	    )
	{
		fprintf( stderr, "\n**************************\n" );
		if( parser->paren_depth < 0 ) {
			fprintf( stderr, "ERROR: paren_depth < 0\n" );
		}
		if( parser->indent < 0 ) {
			fprintf( stderr, "ERROR: indent < 0\n" );
		}
		if( parser->last_brace_indent < 0 ) {
			fprintf( stderr, "ERROR: last_brace_indent < 0\n" );
		}
		fprintf( stderr, "paren_depth = %d\n", 
		         parser->paren_depth );
		fprintf( stderr, "indent = %d\n", 
		         parser->indent );
		fprintf( stderr, "last_brace_indent = %d\n",
		         parser->last_brace_indent );
		fprintf( stderr, "Current line: %d\n",
			 parser->cur_line );
		fprintf( stderr, "Current token:\n" );
		push_token( &(parser->prev_tk), stderr );
		fprintf( stderr, "\n");
		abort();
	}
}
#endif // DEBUG_ON
