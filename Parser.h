#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "SymbolTable.h"

void Parser(FILE* semanticFile);
Token* match(eTOKENS token);

/*All the parsing commands*/
void Parse_PROGRAM();
void Parse_BLOCK();
void Parse_DECLARATIONS();
void Parse_DECLARATIONS_();
void Parse_DECLARATION();
void Parse_VAR_DECLARATION();
SymbolTableEntry* Parse_VAR_DECLARATION_();
int Parse_SIZE();
int Parse_SIMPLE_TYPE();
void Parse_TYPE_DECLARATION();
void Parse_TYPE_INDICATOR(char * type_name);
void Parse_ENUM_TYPE();
void Parse_ID_LIST();
void Parse_ID_LIST_();
void Parse_STRUCTURE_TYPE();
void Parse_FIELDS();
void Parse_FIELDS_();
void Parse_FIELD();
void Parse_STATEMENTS();
void Parse_STATEMENTS_();
void Parse_STATEMENT();
SymbolTableEntry* Parse_VAR_ELEMENT();
// void Parse_VAR_ELEMENT_();
SymbolTableEntry* Parse_VAR_ELEMENT__();
SymbolTableEntry* Parse_KEY();
SymbolTableEntry* Parse_KEY_();
void Parse_CASE_LIST();
void Parse_CASE_LIST_();
void Parse_CASE();
SymbolTableEntry* Parse_KEY_VALUE();
void Parse_FIELD_ACCESS();
SymbolTableEntry* Parse_FIELD_ACCESS_();
SymbolTableEntry* Parse_FIELD_ACCESS__();
SymbolTableEntry* Parse_EXPRESSION(SymbolTableEntry* entry);
SymbolTableEntry* Parse_EXPRESSION_();
SymbolTableEntry* Parse_SIMPLE_EXPRESSION();
SymbolTableEntry* Parse_SIMPLE_EXPRESSION_();

#endif
