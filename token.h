// token.h

#ifndef TOKEN_H
  #define TOKEN_H

typedef enum {
	NOTOKEN = 0,
	IDENT,
	CHAR,
	STRING,
	
	PARENTHESES,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	LBRACKET,
	RBRACKET,
	
	NUM_OF_TOKEN_TYPES
} TokenType;

/* Token points to group of chars in a buffer */
typedef struct Token {
	TokenType    type;
	const char * start;
	const char * end;
} Token;

/* new_tok: retruns initialized token */
Token new_tok();

/* get_token: searches token inside null-terminated string */
Token get_token( const char * str );


/* 
*/
#endif // TOKEN_H
