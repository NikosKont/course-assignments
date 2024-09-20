#include "datastructs.h"

extern FILE *alpha_yyout;
SymTabEntry *buckets[HT_SIZE] = {NULL};
yyscopelist *scopelists[SCOPE_SIZE] = {NULL};

/* symbols */

const char *SymbolType_str[] = {"global variable", "local variable",
                                "formal argument", "user function",
                                "library function"};

SymTabEntry *create_variable(const char *name, unsigned int scope,
                             unsigned int line, int type) {
    Variable *var = malloc(sizeof(Variable));

    // fprintf(alpha_yyout, "Creating variable %s\n", name);

    return sym_create(name, scope, line, 1, var, type);
}
SymTabEntry *create_function(const char *name, VarList *args,
                             unsigned int scope, unsigned int line, int type) {
    Function *func = malloc(sizeof(Function));
    func->arguments = args;
    // TODO: handle
    // func->total_locals = 0;
    // func->iaddress = 0;

    return sym_create(name, scope, line, 1, func, type);
}
char *get_symbol_name(SymTabEntry *symbol) { return strdup(symbol->name); }
unsigned int get_symbol_scope(SymTabEntry *symbol) { return symbol->scope; }
unsigned int get_symbol_line(SymTabEntry *symbol) { return symbol->line; }

char *get_scope_space(SymTabEntry *sym) {
    if (is_function(sym)) return NULL;

    int scp = sym->value.varVal->space;

    if (scp == 0) {
        return "programVar";
    } else if (scp % 2 == 0) {
        return "formalArg";
    } else {
        return "functionLocal";
    }
}

unsigned int get_var_offset(SymTabEntry *symbol) {
    return symbol->value.varVal->offset;
}

/* symtable */

int hash_function(const char *key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) hash = (hash + key[i]) % HT_SIZE;

    return hash;
}
SymTabEntry *sym_create(const char *name, unsigned int scope, unsigned int line,
                        int isActive, void *value, int type) {
    SymTabEntry *sym = (SymTabEntry *)malloc(sizeof(SymTabEntry));
    sym->name = name;
    sym->scope = scope;
    sym->line = line;
    sym->isActive = isActive;
    sym->type = type;

    if (type >= USERFUNC_T)
        sym->value.funcVal = (Function *)value;
    else
        sym->value.varVal = (Variable *)value;

    sym->next = NULL;

    return sym;
}
int sym_insert(SymTabEntry *symbol) {
    if (!symbol) {
        fprintf(stderr, "error: trying to insert null symbol\n");
        return 0;
    }

    int index = hash_function(get_symbol_name(symbol));
    if (!buckets[index]) {
        buckets[index] = symbol;
    } else {
        SymTabEntry *current = buckets[index];
        while (current->next) current = current->next;

        current->next = symbol;
    }
    scope_list_insert(symbol);
    return 1;
}
SymTabEntry *hashtable_lookup(const char *name, unsigned int scope) {
    SymTabEntry *current = buckets[hash_function(name)];

    while (current) {
        if (strcmp(get_symbol_name(current), name) == 0 &&
            get_symbol_scope(current) == scope)
            return current;

        current = current->next;
    }
    return NULL;
}
SymTabEntry *sym_lookup(const char *name, unsigned int scope, int active_only) {
    yyscopelist *current = scopelists[scope];
    while (current) {
        if (strcmp(name, get_symbol_name(current->symbol)) == 0 &&
            !(active_only && !current->symbol->isActive))
            return current->symbol;

        current = current->next;
    }

    return NULL;
}
/* Search the remaining scopes for the symbol */
SymTabEntry *sym_lookup_rem(const char *name, unsigned int scope,
                            int active_only) {
    SymTabEntry *res;

    for (int i = scope - 1; i >= 0; i--)
        if (res = sym_lookup(name, i, active_only)) return res;

    return NULL;
}

/* scopelist */

int scope_list_insert(SymTabEntry *symbol) {
    // weird pointer shenanigans to avoid a couple extra lines of code
    yyscopelist **current = &scopelists[get_symbol_scope(symbol)];
    while (*(current)) current = &((*(current))->next);

    (*current) = (yyscopelist *)malloc(sizeof(yyscopelist));
    (*current)->symbol = symbol;
    (*current)->next = NULL;

    // fprintf(stdout, "\"%s\" [%s] (line %u) (scope %u) \n",
    //         get_symbol_name(symbol),
    //         SymbolType_str[symbol->type],
    //         get_symbol_line(symbol),
    //         get_symbol_scope(symbol));

    return 1;
}
void hide_scope(unsigned int scope) {
    yyscopelist *current = scopelists[scope];

    while (current != NULL) {
        if (current->symbol) current->symbol->isActive = 0;

        current = current->next;
    }

    return;
}

/* print symbols */

void fprint_scope_lists(FILE *out) {
    // fflush(NULL);
    fprintf(out, "\n---------- Scope Lists ----------\n");
    for (int i = 0; i < SCOPE_SIZE; i++) {
        yyscopelist *current = scopelists[i];
        if (!current) continue;
        fprintf(out, "----------------- Scope  #%d -----------------\n", i);

        while (current != NULL) {
            fprintf(out, "[%s] \"%s\" (line %u) (scope %u) \n",
                    SymbolType_str[current->symbol->type],
                    get_symbol_name(current->symbol),
                    get_symbol_line(current->symbol),
                    get_symbol_scope(current->symbol));

            current = current->next;
        }
        fprintf(out, "\n");
    }
    return;
}
void print_scope_lists() { fprint_scope_lists(stdout); }

void fprint_symbol_table(FILE *out) {
    fprintf(out, "---------- Symbol Table ----------\n");
    fflush(NULL);
    // Print hash table contents
    for (int i = 0; i < HT_SIZE; i++) {
        SymTabEntry *current = buckets[i];
        while (current) {
            fprintf(out, "Bucket %d: ", i);

            int line = get_symbol_line(current);
            int scope = get_symbol_scope(current);
            char *name = get_symbol_name(current);
            int offset = current->value.varVal->offset;

            if (is_function(current)) {
                fprintf(out, "Function: %s (scope: %d, line: %d)\n", name,
                        scope, line);
            } else {
                fprintf(out,
                        "Variable: %s (scope: %d, line: %d, scopespace: %s , "
                        "offset: %d) \n",
                        name, scope, line, get_scope_space(current), offset);
            }

            current = current->next;
        }
    }
}
void print_symbol_table() { fprint_symbol_table(stdout); }

/* stack */

alpha_stack_t *create_stack_node(unsigned offset) {
    alpha_stack_t *new_node = (alpha_stack_t *)malloc(sizeof(alpha_stack_t));
    if (new_node) {
        new_node->offset = offset;
        new_node->next = NULL;
        return new_node;
    }
    return NULL;
}

extern alpha_stack_t *offsetStack;
int stack_size;

unsigned pop_stack(alpha_stack_t *top) {
    if (!top) {
        return 0;
    }

    unsigned res = top->offset;
    offsetStack = offsetStack->next;

    return res;
}

unsigned push_stack(alpha_stack_t *top, alpha_stack_t *node) {
    if (!node) return 0;

    if (top == offsetStack) {
        if (!offsetStack) {
            offsetStack = malloc(sizeof(alpha_stack_t));
        }
    }

    node->next = top;
    top = node;
    return 1;
}
