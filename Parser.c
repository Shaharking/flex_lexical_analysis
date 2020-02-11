#pragma warning(disable:6011)

#include "Parser.h"
#include "Token.h"
#include "SymbolTable.h"

SymbolTable* currentTable;

SymbolTableEntry* _errorEntry; 
SymbolTableEntry* _intgerEntry;

FILE* currentSemanticFile;

// create symbol entry spefic for new intger
SymbolTableEntry* createIntgerEntry()
{
	if (_intgerEntry == NULL)
	{
		_intgerEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry*));
		_intgerEntry->role = ROLE_VARIABLE_SIMPLE_INTEGER;
	}
	return _intgerEntry;
}

// create symbol entry for error
SymbolTableEntry* createErrorEntry()
{
	if (_errorEntry == NULL) 
	{
		_errorEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry*));
		_errorEntry->role = ROLE_UNDEFINED;
	}
	return _errorEntry;
}

// Comparing the types two types of SymbolTableEntry
int CompareEntryTypes(SymbolTableEntry* a, SymbolTableEntry* b)
{
	if (a != NULL && b == NULL || a == NULL && b != NULL || a == NULL && b == NULL)
	{
		return 0;
	}
	if (a->role != b->role)
	{
		return 0;
	}
	if (a->role == ROLE_VARIABLE_SIMPLE_INTEGER || a->role == ROLE_VARIABLE_SIMPLE_REAL)
	{
		return 1;
	}
	return a->role_value == b->role_value;
}

Token* match(eTOKENS token, eTOKENS* follow, int followSize)
{
	Token* current_token = next_token();
	// In case it worng, it doesn't handle errors.
	if (current_token->kind != token)
	{
		fprintf(yyout, "Expected token of type %s at line : %d, Actual token of type %s, lexeme : %s . \n",
			getTokenName(token),
			current_token->lineNumber,
			getTokenName(current_token->kind),
			current_token->lexeme);
		// parserErrorHandler(current_token, follow, followSize);
	}
	return current_token;
}

void Parser(FILE* semanticFile)
{
	set_token_pointer_to_head_of_list();
	currentSemanticFile = semanticFile;
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

	SymbolTable* newScopeTable;

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

		// Program there is only one so we set the "root" SymbolTable
		newScopeTable = (SymbolTable*)malloc(sizeof(SymbolTable));
		currentTable = init_symboltable(newScopeTable, currentTable);

		Parse_BLOCK();

		destroy_symboltable(newScopeTable);
		free(newScopeTable);
		currentTable = NULL;
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

	SymbolTable* newScopeTable;

	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_BLOCK:
		fprintf(yyout, "Rule { BLOCK -> block DECLARATIONS begin STATEMENTS end } \n");

		// Creating new Symbol table and adding it to the parent
		newScopeTable = (SymbolTable*)malloc(sizeof(SymbolTable));
		currentTable = init_symboltable(newScopeTable, currentTable);

		Parse_DECLARATIONS();
		match(TOKEN_KEYWORD_BEGIN, follow, 1);
		Parse_STATEMENTS();
		match(TOKEN_KEYWORD_END, follow, 1);

		// When we done with scope, we destory it, and free everything in it.
		currentTable = newScopeTable->parent;
		destroy_symboltable(newScopeTable);
		free(newScopeTable);
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

	SymbolTableEntry* entry;
	char* name;
	fprintf(yyout, "Rule { VAR_DECLARATION -> id : VAR_DECLARATION' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		
		// We verify that id is not already declared in our current scope.
		if (lookup_symboltable_variable(currentTable, current_token->lexeme) == NULL)
		{
			name = current_token->lexeme;
		}
		else
		{
			// if so... print that there is an error
			name = NULL;
			fprintf(currentSemanticFile, "Error: line %d duplicated declaration of %s \n", current_token->lineNumber, current_token->lexeme);
		}

		match(TOKEN_SEPARATION_COLON, follow, 2);
		entry = Parse_VAR_DECLARATION_();
		if (entry != NULL && name != NULL)
		{
			entry->name = name;
			insert_symboltable(currentTable, name, entry);
		}
		
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

SymbolTableEntry* Parse_VAR_DECLARATION_()
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
	SymbolTableEntry* entry;
	entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));

	void* type;
	ArrayVariable* role_value;
	SymbolTableEntry* arrayType;
	fprintf(yyout, "Rule { VAR_DECLARATION' -> integer | real | array [ SIZE ] of SIMPLE_TYPE | type_name } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_INTEGER:
		entry->role = ROLE_VARIABLE_SIMPLE_INTEGER;
		return entry;
	case TOKEN_KEYWORD_REAL:
		entry->role = ROLE_VARIABLE_SIMPLE_REAL;
		return entry;
	case TOKEN_TYPE:
		// in case it's a type, try to find in the struct symbol table 
		type = find_symboltable_struct(currentTable, current_token->lexeme);
		if (type != NULL)
		{
			entry->role = ROLE_TYPE_STRACTURE;
			entry->role_value = type;
			return entry;
		}

		// if it's not found on the struct symbol table, it must be enum
		if (type == NULL)
		{
			type = find_symboltable_enum(currentTable, current_token->lexeme);
		}

		if (type != NULL)
		{
			entry->role = ROLE_TYPE_ENUM;
			entry->role_value = type;
			return entry;
		}

		// if not found then it's undefined, and we print error
		if (type == NULL)
		{
			fprintf(currentSemanticFile, "Error: line %d type %s is not defined\n", current_token->lineNumber, current_token->lexeme);
			entry->role_value = ROLE_UNDEFINED;
			return entry;
		}
		break;
	case TOKEN_KEYWORD_ARRAY:
		role_value = (ArrayVariable*)malloc(sizeof(ArrayVariable));
		match(TOKEN_SEPARATION_BRACKETS_OPEN, follow, 3);
		// Synthesized role that we get from the Parse Size - which return the array size
		role_value->size = Parse_SIZE();
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 3);
		match(TOKEN_KEYWORD_OF, follow, 3);
		// creates role_value type
		arrayType = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		// Synthesized role that get the type of the array
		arrayType->role = Parse_SIMPLE_TYPE();
		role_value->type = arrayType;
		entry->role_value = role_value;
		entry->role = ROLE_VARIABLE_ARRAY;
		return entry;
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 3);
		return NULL;
		break;
	}
}

int Parse_SIZE()
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
		return atoi(current_token->lexeme);
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
	return -1;
}

int Parse_SIMPLE_TYPE()
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
		return SIMPLETYPE_INTEGER;
	case TOKEN_KEYWORD_REAL:
		return SIMPLETYPE_REAL;
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
	Token* temp_token;
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { TYPE_DECLARATION -> type type_name is TYPE_INDICATOR } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_TYPE:
		temp_token = match(TOKEN_TYPE, follow, 2);
		match(TOKEN_KEYWORD_IS, follow, 2);
		Parse_TYPE_INDICATOR(temp_token->lexeme);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_TYPE_INDICATOR(char * type_name)
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

	SymbolTableEntry* entry;

	fprintf(yyout, "Rule { TYPE_INDICATOR -> ENUM_TYPE | STRUCTURE_TYPE } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_ENUM:
		current_token = back_token();
		// Inherited attributes - which gets the id name, we make sure that id not exists, and if so, we insert it enum symboltable
		if (find_symboltable_enum(currentTable, type_name) == NULL)
		{
			entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			entry->name = type_name;
			entry->role = ROLE_TYPE_ENUM;
			entry->role_value = (EnumType*)malloc(sizeof(EnumType));
			((EnumType*)entry->role_value)->values = ht_create(HASH_ENUM_SIZE);
			insert_symboltable_enum(currentTable, type_name, entry);
		}
		else
		{
			// in case it exists, it duplicate
			fprintf(currentSemanticFile, "Error: line %d duplicated struct declaration of %s \n", current_token->lineNumber, current_token->lexeme);
		}
		Parse_ENUM_TYPE(type_name);
		break;
	case TOKEN_KEYWORD_STRUCT:
		current_token = back_token();
		// Inherited attributes - which gets the id name, we make sure that id not exists, and if so, we insert it struct symboltable
		if (find_symboltable_struct(currentTable, type_name) == NULL)
		{
			entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
			entry->name = type_name;
			entry->role = ROLE_TYPE_STRACTURE;
			entry->role_value = (StructType*)malloc(sizeof(StructType));
			((StructType*)entry->role_value)->fields = ht_create(HASH_STRUCT_SIZE);
			insert_symboltable_struct(currentTable, type_name, entry);
		}
		else
		{
			// in case it exists, it duplicate
			fprintf(currentSemanticFile, "Error: line %d duplicated struct declaration of %s \n", current_token->lineNumber, current_token->lexeme);
		}
		Parse_STRUCTURE_TYPE(type_name);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_ENUM_TYPE(char * type_name)
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
		Parse_ID_LIST(type_name);
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 2);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_ID_LIST(char * type_name)
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_CURLY_BRACES_CLOSE;

	Token* current_token = next_token();
	EnumType* enumType;
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { ID_LIST -> id ID_LIST' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		// inherited attribute the id name, check if it exists already or not
		enumType = (EnumType*)find_symboltable_enum(currentTable, type_name);
		if (ht_get(enumType->values, current_token->lexeme) == NULL)
		{
			ht_put(enumType->values, current_token->lexeme, 1);
		}
		else
		{
			fprintf(currentSemanticFile, "Error: line %d duplicated id declaration for enum %s \n", current_token->lineNumber, current_token->lexeme);
		}
		Parse_ID_LIST_(type_name);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_ID_LIST_(char* type_name)
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
		Parse_ID_LIST(type_name);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_STRUCTURE_TYPE(char* type_name)
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
		Parse_FIELDS(type_name);
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 2);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 2);
		break;
	}
}

void Parse_FIELDS(char* type_name)
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
		//TODO: get the the struct and pass it to the child
		Parse_FIELD(type_name);
		Parse_FIELDS_(type_name);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_FIELDS_(char* type_name)
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
		Parse_FIELD(type_name);
		Parse_FIELDS_(type_name);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_FIELD(char * type_name)
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
	SymbolTableEntry* entry;
	SymbolTableEntry* sturctEntry;
	StructType* structType;
	char* name;
	fprintf(yyout, "Rule { FIELD -> id : VAR_DECLARATION' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		name = current_token->lexeme;
		// Create the fields of the new struct, inherited attribute is the struct name
		structType = (StructType*)lookup_symboltable_struct(currentTable, type_name);
		match(TOKEN_SEPARATION_COLON, follow, 2);
		entry = Parse_VAR_DECLARATION_();
		if (entry != NULL && ht_get(structType->fields, name) == NULL)
		{
			entry->name = name;
			ht_put(structType->fields, name, entry);
		}
		else
		{
			fprintf(currentSemanticFile, "Error: line %d duplicated field in struct %s \n", current_token->lineNumber, name);
		}
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
	SymbolTableEntry* entry;
	SymbolTableEntry* expressionEntry;
	SymbolTableEntry* keyEntry;
	Token* temp_token;
	int compareResult;
	if (!current_token)
	{
		return;
	}

	fprintf(yyout, "Rule { VAR_ELEMENT = EXPRESSION | switch ( KEY ) { CASE_LIST ; default : STATEMENTS } | break | block DECLARATIONS begin STATEMENTS end } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		current_token = back_token();
		entry = Parse_VAR_ELEMENT();
		temp_token = match(TOKEN_ASSIGNMENT, follow, 3);
		expressionEntry = Parse_EXPRESSION(entry);
		compareResult = CompareEntryTypes(entry, expressionEntry);
		// Compare the result of the left side and right side of the assignment opreator, to make sure they  are same type.
		if (entry == NULL)
		{
			fprintf(currentSemanticFile, "Error: line %d assignment problems \n", temp_token->lineNumber);
		}
		else if (entry->role == ROLE_TYPE_STRACTURE || entry->role == ROLE_VARIABLE_ARRAY)
		{
			fprintf(currentSemanticFile, "Error: line %d left-hand side of assignment can only be of a simple type or of an enumeration type \n", current_token->lineNumber);
		}
		else if (compareResult == 0)
		{
			fprintf(currentSemanticFile, "Error: line %d assignment of two diffrent types \n", temp_token->lineNumber);
		}
		
		break;
	case TOKEN_KEYWORD_SWITCH:
		match(TOKEN_SEPARATION_PARENTHESES_OPEN, follow, 3);
		keyEntry = Parse_KEY();
		match(TOKEN_SEPARATION_PARENTHESES_CLOSE, follow, 3);
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 3);
		Parse_CASE_LIST(keyEntry);
		match(TOKEN_SEPARATION_SEMICOLON, follow, 3);
		match(TOKEN_KEYWORD_DEFAULT, follow, 3);
		match(TOKEN_SEPARATION_COLON, follow, 3);
		Parse_STATEMENTS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 3);
		break;
	case TOKEN_KEYWORD_BREAK:
		break;
	case TOKEN_KEYWORD_BLOCK:
		/*
			Parse_DECLARATIONS();
			match(TOKEN_KEYWORD_BEGIN, follow, 3);
			Parse_STATEMENTS();
			match(TOKEN_KEYWORD_END, follow, 3);
		*/
		current_token = back_token();
		Parse_BLOCK();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 3);
		break;
	}
}

SymbolTableEntry* Parse_VAR_ELEMENT()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_ASSIGNMENT;

	Token* current_token = next_token();
	SymbolTableEntry* entry;
	if (!current_token)
	{
		return createErrorEntry();
	}
	fprintf(yyout, "Rule { VAR_ELEMENT -> id VAR_ELEMENT'' } \n");

	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		entry = (SymbolTableEntry*)find_symboltable_variable(currentTable, current_token->lexeme);
		entry = Parse_VAR_ELEMENT__(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		entry = createErrorEntry();
		break;
	}
	return entry;
}

/*
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
*/

SymbolTableEntry* Parse_VAR_ELEMENT__(SymbolTableEntry* entry)
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_FIELD_ACCESS;
	first[1] = TOKEN_SEPARATION_BRACKETS_OPEN;


	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_ASSIGNMENT;

	Token* current_token = next_token();
	SymbolTableEntry* temp;
	ArrayVariable* arrayVar;
	int size;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { VAR_ELEMENT'' -> ϵ | [ EXPRESSION ] | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		// Todo expression should return a intger number
		temp = createIntgerEntry();
		temp = Parse_EXPRESSION(temp);
		// Check if you tring to access array propley
		if (entry->role != ROLE_VARIABLE_ARRAY)
		{
			fprintf(currentSemanticFile, "Error: line %d type %s is not array! but trying to accesss it like array\n", current_token->lineNumber, entry->name);
			entry = createErrorEntry();
		}
		else if (temp->role != ROLE_VARIABLE_SIMPLE_INTEGER)
		{
			fprintf(currentSemanticFile, "Error: line %d type %s is array! but index is not integer\n", current_token->lineNumber, entry->name);
			entry = createErrorEntry();
		}
		else
		{
			arrayVar = (ArrayVariable*)entry->role_value;
			entry = arrayVar->type;
		}
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 1);
		return entry;
		break;
	case TOKEN_FIELD_ACCESS:
		// Check to see if you access struct fields propely
		if (entry->role != ROLE_TYPE_STRACTURE)
		{
			fprintf(currentSemanticFile, "Error: line %d type %s is not structure! but trying to accesss field like a struct\n", current_token->lineNumber, entry->name);
		}
		return Parse_FIELD_ACCESS_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		return entry;
		break;
	}
}

SymbolTableEntry* Parse_KEY()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_PARENTHESES_CLOSE;

	Token* current_token = next_token();
	SymbolTableEntry* entry;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { KEY -> id KEY' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		entry = find_symboltable_variable(currentTable, current_token->lexeme);
		entry = Parse_KEY_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		entry = createErrorEntry();
		break;
	}
	return entry;
}

SymbolTableEntry* Parse_KEY_(SymbolTableEntry* entry)
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
	SymbolTableEntry* temp;
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		// Trying to see if you can access array propely
		if (entry->role != ROLE_VARIABLE_ARRAY)
		{
			fprintf(currentSemanticFile, "Error: line %d, Trying to access KEY as array, but it's not an array!\n", current_token->lineNumber);
		}
		temp = Parse_EXPRESSION(NULL);
		if (temp->role != ROLE_VARIABLE_SIMPLE_INTEGER)
		{
			fprintf(currentSemanticFile, "Error: line %d, Trying to access KEY array, but index is not a number!\n", current_token->lineNumber);
		}
		
		if (temp->role == ROLE_VARIABLE_SIMPLE_INTEGER && entry->role == ROLE_VARIABLE_ARRAY)
		{
			entry = ((ArrayVariable*)entry->role_value)->type;
		}
		else
		{
			entry = createErrorEntry();
		}
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 1);
		break;
	case TOKEN_FIELD_ACCESS:
		entry = Parse_FIELD_ACCESS_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
	return entry;
}

void Parse_CASE_LIST(SymbolTableEntry* entry)
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
		Parse_CASE(entry);
		Parse_CASE_LIST_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_CASE_LIST_(SymbolTableEntry *entry)
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_CASE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;

	Token* current_token = next_token();
	SymbolTableEntry* temp;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { CASE_LIST' -> case KEY_VALUE : { STATEMENTS } CASE_LIST' | ϵ } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_CASE:
		temp = Parse_KEY_VALUE(entry);
		// Checking types of KEY_VALUE same as switch var that you used
		if (!CompareEntryTypes(entry, temp))
		{
			fprintf(currentSemanticFile, "Error: line %d KEY_VALUE is not the same type as KEY  \n", current_token->lineNumber);
		}
		match(TOKEN_SEPARATION_COLON, follow, 1);
		match(TOKEN_SEPARATION_CURLY_BRACES_OPEN, follow, 1);
		Parse_STATEMENTS();
		match(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, follow, 1);
		Parse_CASE_LIST_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		break;
	}
}

void Parse_CASE(SymbolTableEntry* entry)
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_KEYWORD_CASE;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	follow[0] = TOKEN_SEPARATION_SEMICOLON;
	follow[1] = TOKEN_KEYWORD_CASE;

	Token* current_token = next_token();
	SymbolTableEntry* temp;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { CASE -> case KEY_VALUE : { STATEMENTS } } \n");
	switch (current_token->kind)
	{
	case TOKEN_KEYWORD_CASE:
		temp = Parse_KEY_VALUE(entry);
		// Checking types of KEY_VALUE same as switch var that you used
		if (!CompareEntryTypes(entry, temp))
		{
			fprintf(currentSemanticFile, "Error: line %d KEY_VALUE is not the same type as KEY  \n", current_token->lineNumber);
		}
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

SymbolTableEntry* Parse_KEY_VALUE(SymbolTableEntry *entry)
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 2);
	first[0] = TOKEN_INTEGER;
	first[1] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	follow[0] = TOKEN_SEPARATION_COLON;

	Token* current_token = next_token();
	SymbolTableEntry* temp;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { KEY_VALUE -> int_num | id } \n");
	switch (current_token->kind)
	{
	case TOKEN_INTEGER:
		temp = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		temp->role = ROLE_VARIABLE_SIMPLE_INTEGER;
		return temp;
		break;
	case TOKEN_IDENTIFIER:
		// Checking types of KEY_VALUE intger or enum
		if (entry == NULL)
		{
			return createErrorEntry();
		}
		if (entry->role != ROLE_TYPE_ENUM)
		{
			printf(currentSemanticFile, "Error: line %d, KEY must be either integer or enumeration type ", current_token->lineNumber);
			return createErrorEntry();
		}
		if (ht_get(((EnumType*)entry->role_value)->values, current_token->lexeme) != NULL)
		{
			return entry;
		}
		return createErrorEntry();
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 1);
		temp = createErrorEntry();
		return temp;
		break;
	}
	return entry;
}

void Parse_FIELD_ACCESS()
{
	eTOKENS* first = (eTOKENS*)malloc(sizeof(eTOKENS) * 1);
	first[0] = TOKEN_IDENTIFIER;

	eTOKENS* follow = (eTOKENS*)malloc(sizeof(eTOKENS) * 0);
	
	Token* current_token = next_token();
	SymbolTableEntry* entry;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { FIELD_ACCESS -> id . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		entry = find_symboltable_variable(currentTable, current_token->lexeme);
		match(TOKEN_FIELD_ACCESS, follow, 0);
		
		Parse_FIELD_ACCESS_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 0);
		break;
	}
}

SymbolTableEntry* Parse_FIELD_ACCESS_(SymbolTableEntry* entry)
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
	StructType* sturctType;
	SymbolTableEntry* tempEntry; 
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { FIELD_ACCESS' -> id FIELD_ACCESS'' } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		// Make sure you access struct propley
		if (entry != NULL && entry->role == ROLE_TYPE_STRACTURE)
		{
			sturctType = (StructType*)entry->role_value;
			tempEntry = (SymbolTableEntry*)ht_get(sturctType->fields, current_token->lexeme);
			if (tempEntry == NULL)
			{
				fprintf(currentSemanticFile, "Error: line %d, trying to access propety %s not exist on struct \n", current_token->lineNumber, current_token->lexeme);
			}
			entry = tempEntry;
		}
		
		return Parse_FIELD_ACCESS__(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 10);
		return entry;
		break;
	}
}

SymbolTableEntry* Parse_FIELD_ACCESS__(SymbolTableEntry* entry)
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
		return Parse_FIELD_ACCESS_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 10);
		return entry;
		break;
	}
}

SymbolTableEntry* Parse_EXPRESSION(SymbolTableEntry* entry)
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
	SymbolTableEntry* type_simple_expression;
	SymbolTableEntry* type_expression_;
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
		// Todo continue from Expression
		type_simple_expression = Parse_SIMPLE_EXPRESSION(entry);
		type_expression_ = Parse_EXPRESSION_(entry);
		// Compare the Synthesized attribute that the math op are done on the types
		if (CompareEntryTypes(type_expression_, type_simple_expression) == 0)
		{
			fprintf(yyout, "Error: line %d expression using multiple types, please only one type \n", current_token->lineNumber);
			type_simple_expression = createErrorEntry();
		}
		return type_simple_expression;
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 4);
		return entry;
		break;
	}
}


SymbolTableEntry* Parse_EXPRESSION_(SymbolTableEntry* entry)
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
	SymbolTableEntry* expressionEntry;
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
		expressionEntry = Parse_EXPRESSION(entry);
		// Compare that math op are done on the types
		if (!(CompareEntryTypes(expressionEntry, entry) == 1 && (expressionEntry->role == ROLE_VARIABLE_SIMPLE_REAL || expressionEntry->role == ROLE_VARIABLE_SIMPLE_INTEGER)))
		{
			entry = createErrorEntry();
			fprintf(yyout,"Error line %d, Arithmetic operations are only applicable to values of simple types (integer or real) ", current_token->lineNumber);
		}
		return entry;
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 4);
		return entry;
		break;
	}
}

SymbolTableEntry* Parse_SIMPLE_EXPRESSION(SymbolTableEntry* entry)
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
	char* enumId;
	EnumType* enumType;
	SymbolTableEntry* simpleExpression;
	SymbolTableEntry* temp;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIMPLE_EXPRESSION -> id SIMPLE_EXPRESSION' | int_num | real_num } \n");
	switch (current_token->kind)
	{
	case TOKEN_IDENTIFIER:
		// verify things done propely
		enumId = current_token->lexeme;

		temp = find_symboltable_variable(currentTable, enumId);
		if (temp != NULL)
		{
			entry = temp;
		}

		entry = Parse_SIMPLE_EXPRESSION_(entry);
		if (entry == NULL)
		{
			return NULL;
		}
		if (entry->role == ROLE_TYPE_ENUM)
		{
			enumType = (EnumType*)entry->role_value;
			if (ht_get(enumType->values, enumId) != NULL)
			{
				return entry;
			}
			else
			{
				return createErrorEntry();
			}
		}
		else if (entry->role != ROLE_VARIABLE_SIMPLE_INTEGER && entry->role != ROLE_VARIABLE_SIMPLE_REAL)
		{
			return createErrorEntry();
		}
		return entry;
		break;
	case TOKEN_INTEGER:
		entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		entry->name = current_token->lexeme;
		entry->role = ROLE_VARIABLE_SIMPLE_INTEGER;
		return entry;
	case TOKEN_REAL:
		entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
		entry->name = current_token->lexeme;
		entry->role = ROLE_VARIABLE_SIMPLE_REAL;
		return entry;
		break;
	default:
		parserErrorHandler(current_token, follow, 8);
		return createErrorEntry();
		// Error handling
		break;
	}
}

SymbolTableEntry* Parse_SIMPLE_EXPRESSION_(SymbolTableEntry* entry)
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
	SymbolTableEntry* temp;
	ArrayVariable* arrayVar;
	if (!current_token)
	{
		return;
	}
	fprintf(yyout, "Rule { SIMPLE_EXPRESSION' -> ϵ | [ EXPRESSION ] | . FIELD_ACCESS' } \n");
	switch (current_token->kind)
	{
	case TOKEN_SEPARATION_BRACKETS_OPEN:
		// Verify you access array propely
		if (entry->role != ROLE_VARIABLE_ARRAY)
		{
			fprintf(currentSemanticFile, "Error: line %d trying to access id as an array \n", current_token->lineNumber);
		}
		// We excpect to fine intger type, so we passed integery as symbol entry
		temp = Parse_EXPRESSION(createIntgerEntry());
		if (temp->role != ROLE_VARIABLE_SIMPLE_INTEGER)
		{
			fprintf(currentSemanticFile, "Error: line %d trying to access array with index that is not integer %s \n", current_token->lineNumber, temp->name);
			entry = createErrorEntry();
		}
		else
		{
			arrayVar = (ArrayVariable*)entry->role_value;
			entry = arrayVar->type;
		}
		match(TOKEN_SEPARATION_BRACKETS_CLOSE, follow, 8);
		return entry;
		break;
	case TOKEN_FIELD_ACCESS:
		if (entry->role != ROLE_TYPE_STRACTURE)
		{
			fprintf(currentSemanticFile, "Error: line %d trying to access id as an struct \n", current_token->lineNumber);
		}
		return Parse_FIELD_ACCESS_(entry);
		break;
	default:
		// Error handling
		parserErrorHandler(current_token, follow, 8);
		return entry;
		break;
	}
}