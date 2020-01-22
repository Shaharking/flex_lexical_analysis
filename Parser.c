#include "Parser.h"
#include "Token.h"

int match(eTOKENS token, eTOKENS* follow, int followSize)
{
	Token* current_token = next_token();
	if (current_token->kind != token)
	{
		parserErrorHandler(current_token, follow, followSize);
		return FALSE;
	}
	return TRUE;
}

void Parser()
{
	set_token_pointer_to_head_of_list();

	Parse_PROGRAM();
}

void Parse_PROGRAM()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_BLOCK;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_END_OF_FILE;

	Token* current_token = next_token();
	current_token = back_token();

	if (!current_token)
	{
		return;
	}

	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_BLOCK:
		// PROGRAM -> BLOCK
		fprintf(yyout, "Rule { PROGRAM -> BLOCK } \n");
		_set_token_pointer_head_warning_from_parse_program_only_();
		Parse_BLOCK();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_BLOCK()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_BLOCK;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_END_OF_FILE;

	Token* current_token = next_token();
	// BLOCK ->  block DECLARATIONS begin STATEMENTS end 
	if (!current_token)
	{
		return;
	}

	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_BLOCK:
		fprintf(yyout, "Rule { BLOCK -> block DECLARATIONS begin STATEMENTS end } \n");
		Parse_DECLARATIONS();
		match(TOKEN_KEYWORD_BEGIN, follow, 1);
		Parse_STATEMENTS();
		match(TOKEN_KEYWORD_END, follow, 1);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_DECLARATIONS()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_IDENTIFIER;
	first[1] = TOKEN_KEYWORD_TYPE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}
	// DECLARATIONS ->  DECLARATION DECLARATIONS’ 
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
	case TOKEN_KEYWORD_TYPE:
		fprintf(yyout, "Rule { DECLARATIONS -> DECLARATION DECLARATIONS' } \n");
		current_token = back_token();
		Parse_DECLARATION();
		Parse_DECLARATIONS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}

}

void Parse_DECLARATIONS_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_SEPARATION_SEMICOLON;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}
	// DECLARATIONS’ -> ; DECLARATIONS | epsilon
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_SEMICOLON:
		fprintf(yyout, "Rule { DECLARATIONS' -> ϵ | ; DECLARATIONS } \n");
		Parse_DECLARATIONS();
		break;
	default:
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_DECLARATION()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_IDENTIFIER;
	first[1] = TOKEN_KEYWORD_TYPE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { DECLARATION -> VAR_DECLARATION | TYPE_DECLARATION } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		Parse_VAR_DECLARATION();
		break;
	case TOKEN_KEYWORD_TYPE:
		current_token = back_token();
		Parse_TYPE_DECLARATION();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_VAR_DECLARATION()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { VAR_DECLARATION -> id : VAR_DECLARATION' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		match(TOKEN_SEPARATION_COLON, follow, 2);
		Parse_VAR_DECLARATION_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_VAR_DECLARATION_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	first[0] = TOKEN_KEYWORD_ARRAY;
	first[1] = TOKEN_TYPE;
	first[2] = TOKEN_KEYWORD_INTEGER;
	first[3] = TOKEN_KEYWORD_REAL;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 3);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;
	follow[2] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { VAR_DECLARATION' -> integer | real | array [ SIZE ] of SIMPLE_TYPE | type_name } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_INTEGER:
	case TOKEN_KEYWORD_REAL:
	case TOKEN_TYPE:
		break;
	case TOKEN_KEYWORD_ARRAY:
		match(TOKEN_SEPARATION_BRACKETS_OPEN, follow, 3);
		Parse_SIZE();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 3);
		match(TOKEN_KEYWORD_OF, follow, 3);
		Parse_SIMPLE_TYPE();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 3);
		break;
	}
}

void Parse_SIZE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_INTEGER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_BRACKETS_CLOSE;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIZE -> int_num } \n");
	switch (current_token->kind)
	{
	case TOKEN_INTEGER:
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_SIMPLE_TYPE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_KEYWORD_INTEGER;
	first[1] = TOKEN_KEYWORD_REAL;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 3);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;
	follow[2] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIMPLE_TYPE -> integer | real } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_INTEGER:
	case TOKEN_KEYWORD_REAL:
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 3);
		break;
	}
}

void Parse_TYPE_DECLARATION()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_TYPE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { TYPE_DECLARATION -> type type_name is TYPE_INDICATOR } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_TYPE:
		match(TOKEN_TYPE, follow, 2);
		match(TOKEN_KEYWORD_IS, follow, 2);
		Parse_TYPE_INDICATOR();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_TYPE_INDICATOR()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_KEYWORD_ENUM;
	first[1] = TOKEN_KEYWORD_STRUCT;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { TYPE_INDICATOR -> ENUM_TYPE | STRUCTURE_TYPE } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_ENUM:
		current_token = back_token();
		Parse_ENUM_TYPE();
		break;
	case TOKEN_KEYWORD_STRUCT:
		current_token = back_token();
		Parse_STRUCTURE_TYPE();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_ENUM_TYPE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_ENUM;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { ENUM_TYPE -> enum { ID_LIST } } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_ENUM:
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 2);
		Parse_ID_LIST();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 2);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_ID_LIST()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { ID_LIST -> id ID_LIST' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		Parse_ID_LIST_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_ID_LIST_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_SEPARATION_COMMA;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { ID_LIST' -> ϵ | , ID_LIST } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_COMMA:
		Parse_ID_LIST();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_STRUCTURE_TYPE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_STRUCT;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_BEGIN;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { STRUCTURE_TYPE -> struct { FIELDS } } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_STRUCT:
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 2);
		Parse_FIELDS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 2);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_FIELDS()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { FIELDS -> FIELD FIELDS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		Parse_FIELD();
		Parse_FIELDS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_FIELDS_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_SEPARATION_SEMICOLON;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;


	Token* current_token = next_token();

	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { FIELDS' -> ; FIELD FIELDS' | ϵ } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_SEMICOLON:
		Parse_FIELD();
		Parse_FIELDS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_FIELD()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { FIELD -> id : VAR_DECLARATION' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		match(TOKEN_SEPARATION_COLON, follow, 2);
		Parse_VAR_DECLARATION_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_STATEMENTS()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	first[0] = TOKEN_KEYWORD_SWITCH;
	first[1] = TOKEN_KEYWORD_BREAK;
	first[2] = TOKEN_KEYWORD_BLOCK;
	first[3] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_KEYWORD_END;
	follow[1] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { STATEMENTS -> STATEMENT STATEMENTS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_SWITCH:
	case TOKEN_KEYWORD_BREAK:
	case TOKEN_KEYWORD_BLOCK:
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		Parse_STATEMENT();
		Parse_STATEMENTS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_STATEMENTS_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_SEPARATION_SEMICOLON;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_KEYWORD_END;
	follow[1] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;


	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { STATEMENTS' -> ; STATEMENT STATEMENTS' | ϵ } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_SEMICOLON:
		Parse_STATEMENT();
		Parse_STATEMENTS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}

}

void Parse_STATEMENT()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	first[0] = TOKEN_KEYWORD_SWITCH;
	first[1] = TOKEN_KEYWORD_BREAK;
	first[2] = TOKEN_KEYWORD_BLOCK;
	first[3] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 3);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_END;
	follow[2] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { VAR_ELEMENT = EXPRESSION | switch ( KEY ) { CASE_LIST ; default : STATEMENTS } | break | block DECLARATIONS begin STATEMENTS end } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		Parse_VAR_ELEMENT();
		match(TOKEN_ASSIGNMENT, follow, 3);
		Parse_EXPRESSION();
		break;
	case TOKEN_KEYWORD_SWITCH:
		match(TOKEN_SEPARATION_PARENTHESES_OPEN, follow, 3);
		Parse_KEY();
		match(TOKEN_SEPARATION_PARENTHESES_CLOSE, follow, 3);
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 3);
		Parse_CASE_LIST();
		match(TOKEN_SEPARATION_SEMICOLON, follow, 3);
		match(TOKEN_KEYWORD_DEFAULT, follow, 3);
		match(TOKEN_SEPARATION_COLON, follow, 3);
		Parse_STATEMENTS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 3);
		break;
	case TOKEN_KEYWORD_BREAK:
		break;
	case TOKEN_KEYWORD_BLOCK:
		Parse_DECLARATIONS();
		match(TOKEN_KEYWORD_BEGIN, follow, 3);
		Parse_STATEMENTS();
		match(TOKEN_KEYWORD_END, follow, 3);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 3);
		break;
	}
}

void Parse_VAR_ELEMENT()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_ASSIGNMENT;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { VAR_ELEMENT -> id VAR_ELEMENT'' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		Parse_VAR_ELEMENT__();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}

}

void Parse_VAR_ELEMENT_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_SEPARATION_BRACKETS_OPEN;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 0);

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { VAR_ELEMENT' -> ϵ | [ EXPRESSION ] } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		Parse_EXPRESSION();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 0);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 0);
		break;
	}
}

void Parse_VAR_ELEMENT__()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_FIELD_ACCESS;
	first[1] = TOKEN_SEPARATION_BRACKETS_OPEN;


	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_ASSIGNMENT;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { VAR_ELEMENT'' -> ϵ | [ EXPRESSION ] | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		Parse_EXPRESSION();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 1);
		break;
	case TOKEN_FIELD_ACCESS:
		Parse_FIELD_ACCESS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_KEY()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_PARENTHESES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { KEY -> id KEY' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		Parse_KEY_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_KEY_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_FIELD_ACCESS;
	first[1] = TOKEN_SEPARATION_BRACKETS_OPEN;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_PARENTHESES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { KEY' -> ϵ | [ EXPRESSION ] | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		Parse_EXPRESSION();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 1);
		break;
	case TOKEN_FIELD_ACCESS:
		Parse_FIELD_ACCESS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_CASE_LIST()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_CASE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { CASE_LIST -> CASE CASE_LIST' } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_CASE:
		current_token = back_token();
		Parse_CASE();
		Parse_CASE_LIST_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_CASE_LIST_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_CASE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { CASE_LIST' -> case KEY_VALUE : { STATEMENTS } CASE_LIST' | ϵ } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_CASE:
		Parse_KEY_VALUE();
		match(TOKEN_SEPARATION_COLON, follow, 1);
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 1);
		Parse_STATEMENTS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 1);
		Parse_CASE_LIST_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_CASE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_CASE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_CASE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { CASE -> case KEY_VALUE : { STATEMENTS } } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_CASE:
		Parse_KEY_VALUE();
		match(TOKEN_SEPARATION_COLON, follow, 2);
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 2);
		Parse_STATEMENTS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 2);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_KEY_VALUE()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_INTEGER;
	first[1] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_COLON;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { KEY_VALUE -> int_num | id } \n");
	switch (current_token->kind)
	{
	case TOKEN_INTEGER:
	case TOKEN_IDENTIFIER:
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_FIELD_ACCESS()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 0);

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { FIELD_ACCESS -> id . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		match(TOKEN_FIELD_ACCESS, follow, 0);
		Parse_FIELD_ACCESS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 0);
		break;
	}
}

void Parse_FIELD_ACCESS_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 10);
	follow[0] = TOKEN_SEPARATION_PARENTHESES_CLOSE;
	follow[1] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_ASSIGNMENT;
	follow[2] = TOKEN_OPERATION_ADDITION;
	follow[3] = TOKEN_OPERATION_SUBTRACTION;
	follow[4] = TOKEN_OPERATION_MULTIPLICATION;
	follow[5] = TOKEN_OPERATION_DIVISION;
	follow[6] = TOKEN_SEPARATION_SEMICOLON;
	follow[7] = TOKEN_KEYWORD_END;
	follow[8] = TOKEN_SEPARATION_BRACKETS_CLOSE;
	follow[9] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { FIELD_ACCESS' -> id FIELD_ACCESS'' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		Parse_FIELD_ACCESS__();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 10);
		break;
	}
}

void Parse_FIELD_ACCESS__()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_FIELD_ACCESS;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 10);
	follow[0] = TOKEN_SEPARATION_PARENTHESES_CLOSE;
	follow[1] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_ASSIGNMENT;
	follow[2] = TOKEN_OPERATION_ADDITION;
	follow[3] = TOKEN_OPERATION_SUBTRACTION;
	follow[4] = TOKEN_OPERATION_MULTIPLICATION;
	follow[5] = TOKEN_OPERATION_DIVISION;
	follow[6] = TOKEN_SEPARATION_SEMICOLON;
	follow[7] = TOKEN_KEYWORD_END;
	follow[8] = TOKEN_SEPARATION_BRACKETS_CLOSE;
	follow[9] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { FIELD_ACCESS'' -> ϵ | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_FIELD_ACCESS:
		Parse_FIELD_ACCESS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 10);
		break;
	}
}

void Parse_EXPRESSION()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 3);
	first[0] = TOKEN_INTEGER;
	first[1] = TOKEN_REAL;
	first[2] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_END;
	follow[2] = TOKEN_SEPARATION_BRACKETS_CLOSE;
	follow[3] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { EXPRESSION -> SIMPLE_EXPRESSION EXPRESSION' } \n");
	switch (current_token->kind)
	{
	case TOKEN_INTEGER:
	case TOKEN_REAL:
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		Parse_SIMPLE_EXPRESSION();
		Parse_EXPRESSION_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 4);
		break;
	}
}


void Parse_EXPRESSION_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	first[0] = TOKEN_OPERATION_ADDITION;
	first[1] = TOKEN_OPERATION_SUBTRACTION;
	first[2] = TOKEN_OPERATION_MULTIPLICATION;
	first[3] = TOKEN_OPERATION_DIVISION;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 4);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_END;
	follow[2] = TOKEN_SEPARATION_BRACKETS_CLOSE;
	follow[3] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { EXPRESSION' -> ϵ | ar_op EXPRESSION } \n");
	switch (current_token->kind)
	{
	case TOKEN_OPERATION_ADDITION:
	case TOKEN_OPERATION_SUBTRACTION:
	case TOKEN_OPERATION_MULTIPLICATION:
	case TOKEN_OPERATION_DIVISION:
		Parse_EXPRESSION();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 4);
		break;
	}
}

void Parse_SIMPLE_EXPRESSION()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 3);
	first[0] = TOKEN_INTEGER;
	first[1] = TOKEN_REAL;
	first[2] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 8);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_SEPARATION_BRACKETS_CLOSE;

	follow[2] = TOKEN_OPERATION_ADDITION;
	follow[3] = TOKEN_OPERATION_SUBTRACTION;
	follow[4] = TOKEN_OPERATION_MULTIPLICATION;
	follow[5] = TOKEN_OPERATION_DIVISION;

	follow[6] = TOKEN_KEYWORD_END;
	follow[7] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIMPLE_EXPRESSION -> id SIMPLE_EXPRESSION' | int_num | real_num } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		Parse_SIMPLE_EXPRESSION_();
		break;
	case TOKEN_INTEGER:
	case TOKEN_REAL:
		break;
	default:
		parserErrorHandler(current_token, follow, 8);
		// Error handling
		break;
	}
}

void Parse_SIMPLE_EXPRESSION_()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_FIELD_ACCESS;
	first[1] = TOKEN_SEPARATION_BRACKETS_OPEN;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 8);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_SEPARATION_BRACKETS_CLOSE;

	follow[2] = TOKEN_OPERATION_ADDITION;
	follow[3] = TOKEN_OPERATION_SUBTRACTION;
	follow[4] = TOKEN_OPERATION_MULTIPLICATION;
	follow[5] = TOKEN_OPERATION_DIVISION;

	follow[6] = TOKEN_KEYWORD_END;
	follow[7] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIMPLE_EXPRESSION' -> ϵ | [ EXPRESSION ] | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		Parse_EXPRESSION();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 8);
		break;
	case TOKEN_FIELD_ACCESS:
		Parse_FIELD_ACCESS_();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 8);
		break;
	}
}