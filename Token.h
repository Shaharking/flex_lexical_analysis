#ifndef TOKEN_H
#define TOKEN_H

#define TOKEN_H_LOADED 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout;

typedef enum eTOKENS
{
	/*Basic types**/
	TOKEN_INTEGER,
	TOKEN_REAL,
	/*Opeartions*/
	TOKEN_OPERATION_ADDITION,
	TOKEN_OPERATION_SUBTRACTION,
	TOKEN_OPERATION_MULTIPLICATION,
	TOKEN_OPERATION_DIVISION,
	/*Assigment */
	TOKEN_ASSIGNMENT,
	/* Field Access */
	TOKEN_FIELD_ACCESS,
	/*id and type_name*/
	TOEKN_IDENTIFIER,
	TOKEN_TYPE,
	/*Keywords*/
	TOKEN_KEYWORD_BLOCK,
	TOKEN_KEYWORD_BEGIN,
	TOKEN_KEYWORD_END,
	TOKEN_KEYWORD_ARRAY,
	TOKEN_KEYWORD_OF,
	TOKEN_KEYWORD_INTEGER,
	TOKEN_KEYWORD_REAL,
	TOKEN_KEYWORD_TYPE,
	TOKEN_KEYWORD_IS,
	TOKEN_KEYWORD_ENUM,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_SWITCH,
	TOKEN_KEYWORD_DEFAULT,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_CASE,
	/* Speartion */
	TOKEN_SEPARATION_COLON,
	TOKEN_SEPARATION_COMMA,
	TOKEN_SEPARATION_SEMICOLON,
	TOKEN_SEPARATION_PARENTHESES_OPEN,
	TOKEN_SEPARATION_PARENTHESES_CLOSE,
	TOKEN_SEPARATION_BRACKETS_OPEN,
	TOKEN_SEPARATION_BRACKETS_CLOSE,
	TOKEN_SEPARATION_CURLY_BRACES_OPEN,
	TOKEN_SEPARATION_CURLY_BRACES_CLOSE
	/*
		Examples of tokens, add/change according to your needs.
		
		Alternative way to define tokens:
		#define TOKEN_INTEGER 1
		#define TOKEN_IF 2
		...........
	*/
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();

#endif