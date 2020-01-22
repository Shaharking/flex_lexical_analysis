#pragma once
#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include "Hashtable.h"

#define HASHSIZE 500

#define ROLE_VARIABLE_SIMPLE 1
#define ROLE_VARIABLE_ARRAY 2
#define ROLE_TYPE_STRACTURE 3
#define ROLE_TYPE_ENUM 4;

typedef struct ArrayVariable {
	char* type;
	int size;
} ArrayVariable;

typedef struct StructType {
	char** fields;
};

typedef struct EnumType {
	int* values;
};

typedef struct SymbolTableEntry {
	char* name;
	int role;
	void* role_value;
} SymbolTableEntry;

typedef struct SymbolTable {
	struct SymbolTable* parent;
	hashtable_t* hashTable;
} SymbolTable;

SymbolTable * init_symboltable(SymbolTable* table, SymbolTable* parent);
void destroy_symboltable(SymbolTable* table);
int insert_symboltable(SymbolTable* table, char* key, SymbolTableEntry* entry);
SymbolTableEntry* lookup_symboltable(SymbolTable* table, char* key);
SymbolTableEntry* find_symboltable(SymbolTable* table, char* key);

#endif