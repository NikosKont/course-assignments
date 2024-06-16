/*  Nikolaos Kontogeorgis - 4655
	HY255 - Assignment 3
	symtablehash.c  */

#include "symtable.h"

#define buckets 509

struct Binding {
	char *key;
	void *value;
	Binding_T next;
};

struct SymTable {
	Binding_T *table;
	unsigned int length;
};

/* Returns a hash code for pcKey. */
static unsigned int SymTable_hash(const char *pcKey) {
	unsigned int hash = 0U;
	char *p;

	for (p = pcKey; *p != '\0'; p++) {
		hash = 31 * hash + *p;
	}
	return (hash % buckets);
}

/* Creates a new SymTable and returns a pointer to it */
SymTable_T SymTable_new(void){
	SymTable_T new;
	unsigned int i;

	new = (SymTable_T)malloc(sizeof(struct SymTable));
	assert(new);
	new->table = (Binding_T *)calloc(buckets, sizeof(Binding_T));
	assert(new->table);

	for (i = 0U; i < buckets; i++) {
		new->table[i] = NULL;
	}
	new->length = 0U;
	return new;
}

/* Frees memory allocated for oSymTable */
void SymTable_free(SymTable_T oSymTable) {
	Binding_T curr = oSymTable->table[0], next;
	unsigned int i;

	if (!oSymTable)
		return;

	for (i = 0U; i < buckets; i++) {
		curr = oSymTable->table[i];
		while (curr) {
			next = (curr)->next;
			free((curr)->key);
			free(curr);
			curr = next;
		}
	}
	free(oSymTable->table);
	free(oSymTable);
}

/* Returns length of oSymTable */
unsigned int SymTable_getLength(SymTable_T oSymTable) {
	assert(oSymTable);
	return oSymTable->length;
}

/* Inserts a new binding (pcKey:pvValue) to oSymTable */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
	Binding_T new;
	unsigned int hash;

	assert(oSymTable && pcKey);
	if (SymTable_contains(oSymTable, pcKey))
		return 0;

	new = (Binding_T)malloc(sizeof(struct Binding));
	assert(new);
	new->key = (char *)malloc((strlen(pcKey) + 1) * sizeof(char));
	assert(new->key);

	oSymTable->length++;
	strcpy(new->key, pcKey);
	new->value = pvValue;
	hash = SymTable_hash(pcKey);
	new->next = oSymTable->table[hash];
	oSymTable->table[hash] = new;

	return 1;
}

/*  If key pcKey exists within a binding of oSymTable, the binding is removed and
	the function returns 1, otherwise no action is carried out and the function
	returns 0  */
int SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
	Binding_T *table, curr, prev;
	unsigned int hash;

	assert(oSymTable && pcKey);
	table = oSymTable->table;

	hash = SymTable_hash(pcKey);

	if (!table[hash])
		return 0;

	curr = table[hash];
	while (curr && strcmp(curr->key, pcKey)) {
		prev = curr;
		curr = curr->next;
	}
	if (!curr)
		return 0;

	if (curr == table[hash])
		table[hash] = curr->next;
	else
		prev->next = curr->next;

	free(curr->key);
	free(curr);
	oSymTable->length--;

	return 1;
}

/*  If key pcKey exists within a binding of oSymTable, the function returns 1
	otherwise the fuinction returns 0  */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
	Binding_T curr;
	unsigned int hash;

	assert(oSymTable && pcKey);

	hash = SymTable_hash(pcKey);
	curr = oSymTable->table[hash];
	while (curr) {
		if (!strcmp(curr->key, pcKey))
			return 1;
		curr = curr->next;
	}
	return 0;
}

/*	If key pcKey exists within a binding ot oSymTable, the function returns the
	value attached to it otherwise it returns NULL  */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
	Binding_T curr;
	unsigned int hash;

	assert(oSymTable && pcKey);

	hash = SymTable_hash(pcKey);
	curr = oSymTable->table[hash];
	while (curr) {
		if (!strcmp(curr->key, pcKey))
			return curr->value;
		curr = curr->next;
	}
	return NULL;
}

/* Applies function pfApply with pvExtra as argument to every binding of oSymTable */
void SymTable_map(SymTable_T oSymTable, 
					void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
					const void *pvExtra) {
	Binding_T *table = oSymTable->table, curr;
	unsigned int i;

	assert(oSymTable && pfApply);

	for (i = 0U; i < buckets; i++) {
		curr = table[i];
		while (curr) {
			pfApply(curr->key, curr->value, (void *)pvExtra);
			curr = curr->next;
		}
	}
}