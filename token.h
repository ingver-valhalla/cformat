// token.h

#ifndef TOKEN_H
  #define TOKEN_H

typedef enum {
	NOTOKEN = 0 ,

	PREPROC     ,
	MUL_COMMENT ,
	COMMENT     ,

	IDENT       ,
	NUM_CONST   ,
	CHR_LIT     ,
	STR_LIT     ,

	IF_KW       ,
	ELSE_KW     ,
	FOR_KW      ,
	WHILE_KW    ,
	DO_KW       ,

	OP          ,
	STRUCT_SEP  ,
	ELLIPSIS    ,

	LPAREN      ,
	RPAREN      ,
	LBRACE      ,
	RBRACE      ,
	LBRACKET    ,
	RBRACKET    ,

	ASSIGN_OP   ,
	COMMA       ,
	SEMICOLON   ,
	COLON       ,
	QUESTION    ,

	LAST_TOKEN_TYPE
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
