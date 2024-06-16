/*  Nikolaos Kontogeorgis - 4655
	HY255 - Assignment 3
	symtable.h  */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct SymTable *SymTable_T;
typedef struct Binding *Binding_T;

/* Creates a new SymTable and returns a pointer to it */
SymTable_T SymTable_new(void);

/* Frees memory allocated for oSymTable */
void SymTable_free(SymTable_T oSymTable);

/* Returns length of oSymTable */
unsigned int SymTable_getLength(SymTable_T oSymTable);

/* Inserts a new binding (pcKey:pvValue) to oSymTable */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* If key pcKey exists within a binding of oSymTable, the binding is removed and
	the function returns 1, otherwise no action is carried out and the function
	returns 0 */
int SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* If key pcKey exists within a binding of oSymTable, the function returns 1
	otherwise the fuinction returns 0 */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* If key pcKey exists within a binding ot oSymTable, the function returns the
	value attached to it otherwise it returns NULL */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Applies function pfApply with pvExtra as argument to every binding of oSymTable */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);