#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <libc.h>

#define HT_SIZE 420
#define SCOPE_SIZE 100
#define is_function(sym) (sym->type >= USERFUNC_T)
#define is_var(sym) (sym->type < USERFUNC_T)
#define var_type(scope) (scope > 0 ? LOCAL_T : GLOBAL_T)
#define IDval(sym) (is_var(sym) ? sym->value.varVal : sym->value.funcVal)

/* symbols */

extern const char* SymbolTypeStr[];
enum SymbolType { GLOBAL_T, LOCAL_T, FORMAL_T, USERFUNC_T, LIBFUNC_T };

typedef struct Variable {
    unsigned int offset;
    unsigned int space;
    unsigned int inFunction;
} Variable;

typedef struct VariableList {
    struct SymbolTableEntry* var;
    struct VariableList* next;
} VarList;

typedef struct returnList {
    unsigned instrLabel;
    struct returnList* next;
} returnList;

typedef struct Function {
    VarList* arguments;
    unsigned int total_locals;
    unsigned int iaddress;
    unsigned int taddress;
    struct returnList* returnlist;
} Function;

/* symtable */

typedef struct SymbolTableEntry {
    const char* name;
    unsigned int scope;
    unsigned int line;
    int isActive;
    union {
        Variable* varVal;
        Function* funcVal;
    } value;

    enum SymbolType type;
    struct SymbolTableEntry* next;
} SymTabEntry;

SymTabEntry* create_variable(const char* name, unsigned int scope,
                             unsigned int line, int type);
SymTabEntry* create_function(const char* name, VarList* args,
                             unsigned int scope, unsigned int line, int type);

char* get_symbol_name(SymTabEntry* symbol);
unsigned int get_symbol_scope(SymTabEntry* symbol);
unsigned int get_symbol_line(SymTabEntry* symbol);
unsigned int get_var_offset(SymTabEntry* symbol);

SymTabEntry* sym_create(const char* name, unsigned int scope, unsigned int line,
                        int isActive, void* value, int type);

int sym_insert(struct SymbolTableEntry* symbol);

struct SymbolTableEntry* hashtable_lookup(const char* name, unsigned int scope);

struct SymbolTableEntry* sym_lookup(const char* name, unsigned int scope,
                                    int active_only);

struct SymbolTableEntry* sym_lookup_rem(const char* name, unsigned int scope,
                                        int active_only);

/* scopelist */

typedef struct ScopeList {
    SymTabEntry* symbol;
    struct ScopeList* next;
} yyscopelist;

int scope_list_insert(struct SymbolTableEntry* symbol);
void hide_scope(unsigned int scope);

/* print symbols */

void fprint_scope_lists(FILE* out);
void print_scope_lists();
void fprint_symbol_table(FILE* out);
void print_symbol_table();

/* stack for scope/offset */

typedef struct stack {
    unsigned offset;
    struct stack* next;
} alpha_stack_t;

struct stack* create_stack_node(unsigned offset);

unsigned pop_stack(alpha_stack_t* top);

unsigned push_stack(alpha_stack_t* top, alpha_stack_t* node);

#endif