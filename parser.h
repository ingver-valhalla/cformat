//parser.h

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "token.h"
typedef struct parser {
	Token prev_tk;

	/* bools */ 
	int empty_line; /* == 1 if in there is nothing in current line of 
	                 * output file, except whitespaces. == 0
	                 * otherwise. */
	int un_op;
	int in_branch;
	int parens_closed;
	/* ----- */

	int cur_line;
	int paren_depth;
	int indent;
	int * brace_indent;    /* stack of brace indents */
	int stack_size;
	int last_brace_indent; /* top of stack */
	
} Parser;

/* new_parser: returns initialized Parser */
Parser new_parser();

/* free_parser: deletes parser */
int free_parser( Parser * p );

/* parse: parsing and writing into file*/
int parse( Parser * parser, FileBuf * buf,FILE * out );

#endif // PARSER_H
