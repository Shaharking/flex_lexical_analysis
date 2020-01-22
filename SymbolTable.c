#pragma once
#include "SymbolTable.h";

SymbolTable* init_symboltable(SymbolTable* table, SymbolTable* parent)
{
	table->parent = parent;
	table->hashTable = ht_create(HASHSIZE);
	return table;
}

void destroy_symboltable(SymbolTable* table)
{
	table->parent = NULL;
	ht_destroy(table->hashTable);
}

int insert_symboltable(SymbolTable* table, char* key, SymbolTableEntry* entry)
{
	void* value = ht_put(table->hashTable, key, entry);
	if (value == NULL) 
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
SymbolTableEntry* lookup_symboltable(SymbolTable* table, char* key)
{
	return (SymbolTableEntry* )ht_get(table->hashTable, key);
}

SymbolTableEntry* find_symboltable(SymbolTable* table, char* key)
{
	SymbolTable* currentTable = table;
	while (currentTable != NULL)
	{
		SymbolTableEntry* entry = (SymbolTableEntry*)lookup_symboltable(currentTable->hashTable, key);
		if (entry == NULL)
		{
			currentTable = currentTable->parent;
		}
		else
		{
			return entry;
		}
	}

	return NULL;
}