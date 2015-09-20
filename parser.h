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
		int same_line; /* == 0 if in there is nothing in current line of 
		                * output file, except whitespaces. == 1
		                * otherwise. */
		TokenType prev_tk;
		int cur_line;
		int brace_depth;
	} state;
} Parser;

/* new_parser: returns initialized Parser */
Parser new_parser();

/* parse: parsing and writing into file*/
int parse( Parser * parser, FILE * out );

#endif // PARSER_H
