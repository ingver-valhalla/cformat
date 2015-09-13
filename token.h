// token.h

#ifndef TOKEN_H
  #define TOKEN_H

typedef enum {
	NOTOKEN = 0 ,

	PREPROC     ,
	MUL_COMMENT ,
	COMMENT     ,
	
	IF_KW       ,
	ELSE_KW     ,
	FOR_KW      ,
	WHILE_KW    ,
	DO_KW       ,
	SWITCH_KW   ,
	CASE_KW     ,
	DEFAULT_KW  ,

	IDENT       ,
	NUM_CONST   ,
	CHR_LIT     ,
	STR_LIT     ,

	LPAREN      ,
	RPAREN      ,
	LBRACE      ,
	RBRACE      ,
	LBRACKET    ,
	RBRACKET    ,

	ASSIGN_OP   ,
	OP          ,
	ELLIPSIS    ,
	STRUCT_SEP  ,

	COMMA       ,
	SEMICOLON   ,
	COLON       ,
	QUESTION    ,

	EOL_TOK     ,
	EOF_TOK     ,
	
	NUM_TOKEN_TYPES,
} TokenType;

/* Token points to group of chars in a buffer */
typedef struct Token {
	TokenType    type;
	char       * start;
	char       * end;
} Token;

/* new_tok: retruns initialized token */
Token new_tok();

/* get_token: searches token inside null-terminated string */
Token get_token( char * str );

#endif // TOKEN_H
