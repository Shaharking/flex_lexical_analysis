#pragma once
#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include "Hashtable.h"

#define HASHSIZE 500
#define HASH_STRUCT_SIZE 120
#define HASH_ENUM_SIZE 120

#define ROLE_VARIABLE_SIMPLE_INTEGER 1
#define ROLE_VARIABLE_SIMPLE_REAL 2
#define ROLE_VARIABLE_ARRAY 3
#define ROLE_TYPE_STRACTURE 4
#define ROLE_TYPE_ENUM 5
#define ROLE_UNDEFINED 6

#define SIMPLETYPE_INTEGER 1
#define SIMPLETYPE_REAL 2

typedef struct SymbolTableEntry {
	char* name;
	int role;
	void* role_value;
} SymbolTableEntry;

typedef struct ArrayVariable {
	SymbolTableEntry* type;
	int size;
} ArrayVariable;

typedef struct StructType {
	hashtable_t* fields;
} StructType;

typedef struct EnumType {
	hashtable_t* values;
} EnumType;

typedef struct SymbolTable {
	struct SymbolTable* parent;
	hashtable_t* hashTable;
	hashtable_t* structTable;
	hashtable_t* enumTable;
} SymbolTable;

SymbolTable * init_symboltable(SymbolTable* table, SymbolTable* parent);
void destroy_symboltable(SymbolTable* table);
int insert_symboltable(SymbolTable* table, char* key, SymbolTableEntry* entry);
int insert_symboltable_enum(SymbolTable* table, char* key, SymbolTableEntry* entry);
int insert_symboltable_struct(SymbolTable* table, char* key, SymbolTableEntry* entry);

SymbolTableEntry* lookup_symboltable_variable(SymbolTable* table, char* key);
StructType* lookup_symboltable_struct(SymbolTable* table, char* key);
EnumType* lookup_symboltable_enum(SymbolTable* table, char* key);

SymbolTableEntry* find_symboltable_variable(SymbolTable* table, char* key);
StructType* find_symboltable_struct(SymbolTable* table, char* key);
EnumType* find_symboltable_enum(SymbolTable* table, char* key);

#endif