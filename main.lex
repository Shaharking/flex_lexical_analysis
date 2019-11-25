%option noyywrap

%{
#include "Token.h"
/*
create_and_store_token(TOKEN_KEYWORD_BLOCK, yytext, line);
*/

int line=1;

%}

%s COMMENT

DIGIT       [0-9]
ALPHA	    [a-zA-Z]
ALPHA_NUMERIC [a-zA-Z0-9]
ADDITION    [\+]
SUBTRACTION [\-]
MULTIPLICATION [\*]
DIVISION [\/]
ASSIGNMENT  [=]
ACCESS_FIELD_STRUCTURES [\.]
COLON [\:]
COMMA [\,]
SEMICOLON [\;]
PARENTHESES_OPEN [\(]
PARENTHESES_CLOSE [\)]
BRACKETS_OPEN    [\[]
BRACKETS_CLOSE   [\]]
CURLY_BRACES_OPEN    [\{]
CURLY_BRACES_CLOSE    [\}]
IDENTIFIER  {ALPHA}({ALPHA_NUMERIC}|{ALPHA_NUMERIC}+_{ALPHA_NUMERIC}+)*(_{ALPHA_NUMERIC})?
BLOCK   "block"
BEGIN   "begin"
END     "end"
ARRAY   "array"
OF  "of"
INTEGER "integer"
REAL    "real"
TYPE    "type"
IS  "is"
ENUM    "enum"
STRUCT  "struct"
SWITCH  "switch"
DEFAULT "default"
BREAK   "break"
CASE    "case"
LINE [\n]

%%

\$\$.*              { }
{BLOCK}             { create_and_store_token(TOKEN_KEYWORD_BLOCK, yytext, line); }
{BEGIN}             { create_and_store_token(TOKEN_KEYWORD_BEGIN, yytext, line); }
{END}             { create_and_store_token(TOKEN_KEYWORD_END, yytext, line);  }
{ARRAY}             { create_and_store_token(TOKEN_KEYWORD_ARRAY, yytext, line);  }
{OF}             { create_and_store_token(TOKEN_KEYWORD_OF, yytext, line);  }
{INTEGER}             { create_and_store_token(TOKEN_KEYWORD_INTEGER, yytext, line);  }
{REAL}             { create_and_store_token(TOKEN_KEYWORD_REAL, yytext, line);  }
{TYPE}             { create_and_store_token(TOKEN_KEYWORD_TYPE, yytext, line);  }
{IS}             { create_and_store_token(TOKEN_KEYWORD_IS, yytext, line);  }
{ENUM}             { create_and_store_token(TOKEN_KEYWORD_ENUM, yytext, line);  }
{STRUCT}             { create_and_store_token(TOKEN_KEYWORD_STRUCT, yytext, line);  }
{SWITCH}             { create_and_store_token(TOKEN_KEYWORD_SWITCH, yytext, line);  }
{DEFAULT}             { create_and_store_token(TOKEN_KEYWORD_DEFAULT, yytext, line);  }
{BREAK}             { create_and_store_token(TOKEN_KEYWORD_BREAK, yytext, line);  }
{CASE}             { create_and_store_token(TOKEN_KEYWORD_CASE, yytext, line);  }
{COLON}  { create_and_store_token(TOKEN_SEPARATION_COLON, yytext, line);  }
{COMMA}  { create_and_store_token(TOKEN_SEPARATION_COMMA, yytext, line);  }
{SEMICOLON}  { create_and_store_token(TOKEN_SEPARATION_SEMICOLON, yytext, line);  }
{PARENTHESES_OPEN}  { create_and_store_token(TOKEN_SEPARATION_PARENTHESES_OPEN, yytext, line);  }
{PARENTHESES_CLOSE}  { create_and_store_token(TOKEN_SEPARATION_PARENTHESES_CLOSE, yytext, line);  }
{BRACKETS_OPEN}  { create_and_store_token(TOKEN_SEPARATION_BRACKETS_OPEN, yytext, line);  }
{BRACKETS_CLOSE}  { create_and_store_token(TOKEN_SEPARATION_BRACKETS_CLOSE, yytext, line);  }
{CURLY_BRACES_OPEN}  { create_and_store_token(TOKEN_SEPARATION_CURLY_BRACES_OPEN, yytext, line);  }
{CURLY_BRACES_CLOSE}  { create_and_store_token(TOKEN_SEPARATION_CURLY_BRACES_CLOSE, yytext, line);  }
{ADDITION}  { create_and_store_token(TOKEN_OPERATION_ADDITION, yytext, line);  }
{SUBTRACTION}  { create_and_store_token(TOKEN_OPERATION_SUBTRACTION, yytext, line);  }
{MULTIPLICATION}  { create_and_store_token(TOKEN_OPERATION_MULTIPLICATION, yytext, line);  }
{DIVISION}  { create_and_store_token(TOKEN_OPERATION_DIVISION, yytext, line);  }
{ASSIGNMENT}  { create_and_store_token(TOKEN_ASSIGNMENT, yytext, line);  }
{ACCESS_FIELD_STRUCTURES}  { create_and_store_token(TOKEN_FIELD_ACCESS, yytext, line);  }
_{IDENTIFIER}       { create_and_store_token(TOKEN_TYPE, yytext, line); } //type_name
{IDENTIFIER}        { create_and_store_token(TOEKN_IDENTIFIER, yytext, line); } //Identifyer
[1-9]({DIGIT})*\.({DIGIT})+              { create_and_store_token(TOKEN_REAL, yytext, line); }
[1-9]({DIGIT})*                        { create_and_store_token(TOKEN_INTEGER, yytext, line);  }
\n 			{line++;}
[ \t]         {}
.       	{fprintf(yyout ,"Character %s in line: %d does not begin any legal token in the language. \n", yytext, line );}

%%

int main(int argc, char **argv )
{
    yyin=fopen("C:\\temp\\test1.txt","r");
	yyout=fopen("C:\\temp\\test1_311402739_205669625_lex.txt","w");
	//yyout = stdout;

	if(yyin==0)
	{
		fprintf(yyout,"Unable to open the file!");
	}

    yylex();

    fclose(yyin);
    fclose(yyout);

	line = 1;

	yyin=fopen("C:\\temp\\test2.txt","r");
	yyout=fopen("C:\\temp\\test2_311402739_205669625_lex.txt","w");

	if (yyin == 0)
	{
		fprintf(yyout,"Unable to open the file!");
	}

	yyrestart(yyin);

	yylex();

    fclose(yyin);
    fclose(yyout);
}
