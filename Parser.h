#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "Token.h"

void Parser();
int match(eTOKENS token);

/*All the parsing commands*/
void Parse_PROGRAM();
void Parse_BLOCK();
void Parse_DECLARATIONS();
void Parse_DECLARATIONS_();
void Parse_DECLARATION();
void Parse_VAR_DECLARATION();
void Parse_VAR_DECLARATION_();
void Parse_SIZE();
void Parse_SIMPLE_TYPE();
void Parse_TYPE_DECLARATION();
void Parse_TYPE_INDICATOR();
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
void Parse_VAR_ELEMENT();
void Parse_VAR_ELEMENT_();
void Parse_VAR_ELEMENT__();
void Parse_KEY();
void Parse_KEY_();
void Parse_CASE_LIST();
void Parse_CASE_LIST_();
void Parse_CASE();
void Parse_KEY_VALUE();
void Parse_FIELD_ACCESS();
void Parse_FIELD_ACCESS_();
void Parse_FIELD_ACCESS__();
void Parse_EXPRESSION();
void Parse_EXPRESSION_();
void Parse_SIMPLE_EXPRESSION();
void Parse_SIMPLE_EXPRESSION_();

#endif
