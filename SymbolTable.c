#pragma once
#include "SymbolTable.h";

SymbolTable* init_symboltable(SymbolTable* table, SymbolTable* parent)
{
	table->parent = parent;
	table->hashTable = ht_create(HASHSIZE);
	table->structTable = ht_create(HASH_STRUCT_SIZE);
	table->enumTable = ht_create(HASH_ENUM_SIZE);
	return table;
}

void destroy_symboltable(SymbolTable* table)
{
	table->parent = NULL;
	ht_destroy(table->hashTable);
	ht_destroy(table->structTable);
	ht_destroy(table->enumTable);
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

int insert_symboltable_enum(SymbolTable* table, char* key, SymbolTableEntry* entry)
{
	void* value = ht_put(table->enumTable, key, entry);
	if (value == NULL)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int insert_symboltable_struct(SymbolTable* table, char* key, SymbolTableEntry* entry)
{
	void* value = ht_put(table->structTable, key, entry);
	if (value == NULL)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

StructType* lookup_symboltable_struct(SymbolTable* table, char* key)
{
	SymbolTableEntry* structElement = ht_get(table->structTable, key);
	if (structElement)
	{
		return (StructType*)structElement->role_value;
	}
	return NULL;
}


SymbolTableEntry* lookup_symboltable_variable(SymbolTable* table, char* key)
{
	return (SymbolTableEntry* )ht_get(table->hashTable, key);
}

EnumType* lookup_symboltable_enum(SymbolTable* table, char* key)
{
	SymbolTableEntry* entry = (SymbolTableEntry * )ht_get(table->enumTable, key);
	if (entry != NULL)
	{
		EnumType* result = (EnumType*)entry->role_value;
		return result;
	}
	return NULL;

}

SymbolTableEntry* find_symboltable_variable(SymbolTable* table, char* key)
{
	SymbolTable* currentTable = table;
	while (currentTable != NULL)
	{
		SymbolTableEntry* entry = (SymbolTableEntry*)lookup_symboltable_variable(currentTable, key);
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

StructType* find_symboltable_struct(SymbolTable* table, char* key)
{
	SymbolTable* currentTable = table;
	while (currentTable != NULL)
	{
		StructType* entry = lookup_symboltable_struct(currentTable, key);
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

EnumType* find_symboltable_enum(SymbolTable* table, char* key)
{
	SymbolTable* currentTable = table;
	while (currentTable != NULL)
	{
		EnumType* entry = lookup_symboltable_enum(currentTable, key);
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