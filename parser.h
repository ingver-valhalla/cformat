//parser.h

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "token.h"
typedef struct parser {
	FileBuf * buf;

	struct parser_state {
		Token prev_tk;
		Token prev_nonwhite_tk;

		/* bools */ 
		int empty_line; /* == 1 if in there is nothing in current line of 
		                * output file, except whitespaces. == 0
		                * otherwise. */
		int un_op;
		int in_branch;
		/* ----- */

		int cur_line;
		int paren_depth;
		int indent;
		int last_brace_indent;
		int of_1l_branches; /* track the amount of one line 
		                         * branches */
	} state;
} Parser;

/* new_parser: returns initialized Parser */
Parser new_parser();

/* parse: parsing and writing into file*/
int parse( Parser * parser, FILE * out );

#endif // PARSER_H
