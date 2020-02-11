#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0;
Node* currentNode = NULL;
Node* head = NULL;
Node* tail = NULL;

#define TOKEN_ARRAY_SIZE 100

const char* stringTokenNames[] = {
	"INTEGER",
	"REAL",
	"OPERATION_ADDITION",
	"OPERATION_SUBTRACTION",
	"OPERATION_MULTIPLICATION",
	"OPERATION_DIVISION",
	"ASSIGNMENT",
	"FIELD_ACCESS",
	"IDENTIFIER",
	"TYPE",
	"KEYWORD_BLOCK",
	"KEYWORD_BEGIN",
	"KEYWORD_END",
	"KEYWORD_ARRAY",
	"KEYWORD_OF",
	"KEYWORD_INTEGER",
	"KEYWORD_REAL",
	"KEYWORD_TYPE",
	"KEYWORD_IS",
	"KEYWORD_ENUM",
	"KEYWORD_STRUCT",
	"KEYWORD_SWITCH",
	"KEYWORD_DEFAULT",
	"KEYWORD_BREAK",
	"KEYWORD_CASE",
	"COLON",
	"COMMA",
	"SEMICOLON",
	"PARENTHESES_OPEN",
	"PARENTHESES_CLOSE",
	"BRACKETS_OPEN",
	"BRACKETS_CLOSE",
	"CURLY_BRACES_OPEN",
	"CURLY_BRACES_CLOSE",
	"TOKEN_END_OF_FILE"
};

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	int length = strlen(lexeme)+1;
	
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));
		head = currentNode;
		tail = currentNode;
		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
			tail = currentNode;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;	
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
	fprintf(yyout, "Token of type %s, lexeme: %s, found in line: %d\n", getTokenName(kind), lexeme, numOfLine);
	#ifdef _WIN32
		strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
	#else
		strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	#endif		

}

/* reset store token */
void reset_store_token()
{
	Node* node = tail;
	Node* next_node;
	while (node != NULL)
	{
		next_node = node->prev;
		free(node->tokensArray);
		free(node);
		node = next_node;
	}
	currentNode = NULL;
	head = NULL;
	tail = NULL;
	currentIndex = 0;
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token() 
{ 
	if (!currentNode || currentIndex == 0 && currentNode == head)
	{
		return NULL;
	}

	if (currentIndex == 0)
	{
		currentIndex = TOKEN_ARRAY_SIZE;
		currentNode = currentNode->prev;
		return back_token();
	}
	
	currentIndex = currentIndex - 1;
	return &(currentNode->tokensArray[currentIndex]);
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token *next_token() 
{ 

	if (!currentNode || currentNode == tail && currentIndex == 99)
	{
		return NULL;
	}

	if (currentIndex == TOKEN_ARRAY_SIZE - 1)
	{
		currentIndex = -1;
		currentNode = currentNode->next;
		return next_token();
	}
	else
	{
		currentIndex = currentIndex + 1;
		return &(currentNode->tokensArray[currentIndex]);
	}
}

void set_token_pointer_to_head_of_list()
{
	currentNode = head;
	currentIndex = 0;
}

void _set_token_pointer_head_warning_from_parse_program_only_()
{
	// You can never get to the tokens[0] of the head....
	// So you call this "weird" function from the parse program before you call 'Parse_BLOCK', So you can get the "block" from next_token only.
	currentIndex = -1;
}

Token* current_token()
{
	if (!currentNode)
	{
		return NULL;
	}
	return &(currentNode->tokensArray[currentIndex]);
}

void parserErrorHandler(Token* current_token, eTOKENS* follow, int followSize)
{
	Token* current_token_saver = current_token;
	Token* token = current_token;
	int loop = 0;
	bool hasError = TRUE;
	bool isPartOfFollow = TRUE;

	while (token && hasError)
	{
		for (loop = 0; loop < followSize; loop++)
		{
			if (token->kind == follow[loop])
			{
				hasError = FALSE;
				break;
			}
		}

		if (hasError == FALSE)
		{
			break;
		}

		isPartOfFollow = FALSE;
		token = next_token();
	}
	if (token && isPartOfFollow == FALSE)
	{
		// it's not part of the follow group -> so there is syntax error
		/**Expected token of type '{expectedToken.type}' at line : {token.line},
			Actual token of type '{token.type}', lexeme : '{token.lexeme}**/
		fprintf(yyout, "Expected token of type ");
		for (loop = 0; loop < followSize; loop++)
		{
			fprintf(yyout, "%s", getTokenName(follow[loop]));
			if (loop != followSize - 1)
			{
				fprintf(yyout, ",");
			}
		}
		fprintf(yyout, " at line : %d, Actual token of type %s, lexeme : %s . \n", current_token_saver->lineNumber, getTokenName(current_token_saver->kind), current_token_saver->lexeme);
	}
	if (!token)
	{
		return 1;
	}
	else
	{
		current_token = back_token();
	}
	/*
	if (isPartOfFollow == FALSE)
	{
		token = back_token();
	}
	else
	{
		return 1;
	}*/
}

char* getTokenName(eTOKENS token)
{
	return stringTokenNames[token];
}