#include <math.h>

#include "alphavm.h"
#include "datastructs.h"
#include "quads.h"
#include "tcode.h"

extern FILE* alpha_yyout;
extern int alpha_yylineno;
extern int scope;
extern int inFunction;
extern int search_array();

unsigned tempcounter = 0;
unsigned hidfunccounter = 0;

struct lc_stack_t {
    struct lc_stack_t* next;
    unsigned counter;
} *lcs_top = 0, *lcs_bottom = 0;

void push_loopcounter() {
    struct lc_stack_t* new_node =
        (struct lc_stack_t*)malloc(sizeof(struct lc_stack_t));
    new_node->next = lcs_top;
    new_node->counter = 0;
    lcs_top = new_node;
    if (!lcs_bottom) lcs_bottom = lcs_top;
}

void pop_loopcounter() {
    struct lc_stack_t* tmp = lcs_top;
    lcs_top = lcs_top->next;
    free(tmp);
    if (!lcs_top) lcs_bottom = 0;
}

#define loopcounter (lcs_top->counter)

#define yyline alpha_yylineno

void printrule(const char* rule) {
    fprintf(alpha_yyout, "%s: line %d\n", rule, yyline);
}

char* lib_funcs[] = {"print",
                     "input",
                     "objectmemberkeys",
                     "objecttotalmembers",
                     "objectcopy",
                     "totalarguments",
                     "argument",
                     "typeof",
                     "strtonum",
                     "sqrt",
                     "cos",
                     "sin"};
int is_libfunc(const char* name) {
    return search_array(name, lib_funcs, 12) != -1;
}

SymTabEntry* new_var(const char* name, int type) {
    SymTabEntry* sym = create_variable(name, scope, yyline, type);
    sym->value.varVal->space = currscopespace();
    sym->value.varVal->offset = currscopeoffset();
    sym->value.varVal->inFunction = inFunction;
    sym_insert(sym);
    increase_curr_offset();

    return sym;
}
SymTabEntry* new_func(const char* name, int type, VarList* args) {
    SymTabEntry* sym = create_function(name, args, scope, yyline, type);
    sym_insert(sym);

    return sym;
}

// double fmod(double x, double y) { return x - (int)(x / y) * y; }

char* newtempname() {
    char name[50] = "$", idx[10];
    sprintf(idx, "%d", tempcounter);
    strcat(name, idx);
    tempcounter++;
    return strdup(name);
}
void resettempcounter() { tempcounter = 0; }

SymTabEntry* newtemp() {
    char* name = newtempname();

    SymTabEntry* sym = sym_lookup(name, scope, 1);
    if (sym) return sym;
    sym = sym_lookup_rem(name, scope, 1);
    if (sym) return sym;
    // fprintf(stdout, "NEW TEMP ADDED\n");

    sym = new_var(name, LOCAL_T);
    // sym->value.varVal->space = currscopespace();
    // sym->value.varVal->offset = currscopeoffset();

    return sym;
}

SymTabEntry* newtemp_reuse(struct expr* e, ...) {
    va_list args;
    va_start(args, e);
    while (e) {
        if (istempexpr(e)) {
            va_end(args);
            return e->sym;
        }
        // while ((e = e->next)) {
        //     if (istempexpr(e)) {
        //         va_end(args);
        //         return e->sym;
        //     }
        // }
        e = va_arg(args, struct expr*);
    }
    va_end(args);

    return newtemp();
}

struct call* newcall(struct expr* elist, int method, char* name) {
    struct call* c = malloc(sizeof(struct call));
    // memset(c, 0, sizeof(struct call));
    c->elist = elist;
    c->method = method;
    c->name = strdup(name);
    return c;
}

struct stmt* make_stmt() {
    struct stmt* s = (struct stmt*)malloc(sizeof(struct stmt));
    memset(s, 0, sizeof(struct stmt));
    s->breaklist = 0;
    s->contlist = 0;
    s->returnlist = 0;
    return s;
}

expr* list_append(expr* list, expr* e) {
    if (!list) return e;
    expr* curr = list;

    while (curr->next) curr = curr->next;

    curr->next = e;

    return list;
}

// TODO: check where needs to be used
int check_lval(struct expr* lval) {
    assert(lval);

    if is_function (lval->sym) {
        fmt_error("Cannot use function \"%s\" as lvalue",
                  strdup(lval->sym->name));
        // return 0;
        exit(1);
    }

    return 1;
}

expr* lvalue_expr(struct SymbolTableEntry* sym) {
    assert(sym);
    expr* e = (expr*)malloc(sizeof(expr));
    memset(e, 0, sizeof(expr));

    e->next = (expr*)0;
    e->sym = sym;

    switch (sym->type) {
        case GLOBAL_T:
        case LOCAL_T:
        case FORMAL_T:
            e->type = var_e;
            break;
        case USERFUNC_T:
            e->type = programfunc_e;
            break;
        case LIBFUNC_T:
            e->type = libraryfunc_e;
            break;
        default:
            assert(0);
    }
    return e;
}

SymTabEntry* handle_lval(char* name) {
    SymTabEntry* sym = sym_lookup(name, scope, 1);

    if (sym) return sym;

    sym = sym_lookup_rem(name, scope, 1);

    if (sym == NULL) return new_var(name, var_type(scope));

    if (sym->type == GLOBAL_T || is_function(sym)) return sym;

    if (currscopespace() == functionlocal &&
        inFunction > sym->value.varVal->inFunction && inFunction > 1) {
        // printf("inFunction: %d, scope: %d, sym->scope: %d, sym->name: %s\n",
        // inFunction, scope,
        //        sym->scope, sym->name);
        if (sym->type == FORMAL_T) {
            fmt_error(
                "Cannot access formal argument \"%s\" within another function",
                strdup(name));
            // return NULL;
            exit(1);
        } else if (sym->type == LOCAL_T) {
            fmt_error("Cannot access local variable \"%s\" within function",
                      strdup(name));
            // return NULL;
            exit(1);
        }
    }

    return sym;
}
SymTabEntry* handle_local_lval(char* name) {
    SymTabEntry* sym = sym_lookup(name, scope, 1);

    if (sym) return sym;

    if (is_libfunc(name)) {
        error("Cannot overwrite library function");
        return NULL;
    }

    return new_var(name, LOCAL_T);
}
SymTabEntry* handle_global_lval(char* name) {
    SymTabEntry* sym = sym_lookup(name, 0, 1);
    if (sym != NULL) return sym;

    fmt_error("Undefined global variable \"%s\"", strdup(name));
    // return NULL;
    exit(1);
}

VarList* handle_idlist_single(char* name) {
    if (is_libfunc(name)) {
        fmt_error("Cannot overwrite library function \"%s\"", strdup(name));
        exit(1);
    }
    scope++;
    if (sym_lookup(name, scope, 1) != NULL) {
        fmt_error("Formal argument \"%s\" already declared", strdup(name));
        exit(1);
    }

    SymTabEntry* var = new_var(name, FORMAL_T);
    scope--;

    VarList* l = malloc(sizeof(VarList));
    l->var = var;
    l->next = NULL;
    return l;
}
VarList* handle_idlist_multiple(VarList* list, char* name) {
    VarList* head = handle_idlist_single(name);
    head->next = list;
    return head;
}

int check_funcdef(char* name) {
    if (sym_lookup(name, scope, 1) != NULL) {
        fmt_error(" \"%s\" already defined in scope", strdup(name));
        // return 0;
        exit(1);
    }
    if (is_libfunc(name)) {
        fmt_error("Cannot overwrite library function \"%s\"", strdup(name));
        // return 0;
        exit(1);
    }
    return 1;
}

SymTabEntry* handle_funcdef(char* name, VarList* args) {
    // check function collisions
    if (!check_funcdef(name)) return NULL;
    SymTabEntry* sym = new_func(name, USERFUNC_T, args);

    // check arguments collisions
    scope++;
    while (args != NULL) {
        char* arg_name = strdup(args->var->name);
        if (!check_funcdef(arg_name)) continue;
        new_var(strdup(arg_name), FORMAL_T);
        args = args->next;
    }
    scope--;

    return sym;
}
char* newhidfuncname() {
    char name[50] = "_func", idx[10];
    sprintf(idx, "%d", hidfunccounter++);
    strcat(name, idx);
    return strdup(name);
}
SymTabEntry* handle_funcdef_anon(VarList* args) {
    return handle_funcdef(newhidfuncname(), args);
}

void handle_funcdef_lib(char* name) {
    new_func(name, LIBFUNC_T, NULL)->line = 0;
}

expr* handle_assign(struct expr* lval, struct expr* rval) {
    printrule("assignment");
    if (!check_lval(lval)) return NULL;

    struct expr* res = NULL;

    if (lval->type == tableitem_e) {
        emit(tablesetelem, lval->index, rval, lval);
        res = emit_iftableitem(lval);
        res->type = assignexpr_e;
        return res;
    }

    rval = short_circuit(rval);

    emit(assign, rval, NULL, lval);
    res = newexpr(assignexpr_e);
    res->sym = newtemp_reuse(lval, NULL);
    emit(assign, lval, NULL, res);

    return res;
}

int arithm_constint(int x, iopcode op, int y) {
    int res;
    switch (op) {
        case add:
            res = x + y;
            break;
        case sub:
            res = x - y;
            break;
        case mul:
            res = x * y;
            break;
        case Div:
            res = x / y;
            break;
        case mod:
            res = x % y;
            break;
        default:
            assert(0);
    }
    return res;
}

double arithm_constreal(double x, iopcode op, double y) {
    switch (op) {
        case add:
            return x + y;
        case sub:
            return x - y;
        case mul:
            return x * y;
        case Div:
            return x / y;
        case mod:
            return fmod(x, y);
        default:
            assert(0);
    }
}

expr* handle_arithmexpr(struct expr* arg1, iopcode op, struct expr* arg2) {
    printrule("arithmexpr");
    if (!arg1 || !arg2) return NULL;
    check_arith(arg1, "arithmexpr");
    check_arith(arg2, "arithmexpr");

    expr* res;

    res = lvalue_expr(newtemp_reuse(arg1, arg2, NULL));

    emit(op, arg1, arg2, res);

    return res;
}

int rel_constbool(int x, iopcode op, int y) {
    switch (op) {
        case if_eq:
            return x == y;
        case if_noteq:
            return x != y;
        case if_lesseq:
            return x <= y;
        case if_greatereq:
            return x >= y;
        case if_less:
            return x < y;
        case if_greater:
            return x > y;
        default:
            assert(0);
    }
}

expr* handle_relexpr(struct expr* arg1, iopcode op, struct expr* arg2) {
    printrule("relexpr");
    if (!arg1 || !arg2) return NULL;

    arg1 = short_circuit(arg1);
    arg2 = short_circuit(arg2);

    expr* res = newexpr(boolexpr_e);
    res->sym = newtemp_reuse(arg1, arg2, NULL);

    emit_l(op, arg1, arg2, NULL, 0);
    emit_jump();

    res->truelist = newlist(nextquadlabel() - 2);
    res->falselist = newlist(nextquadlabel() - 1);

    return res;
}

expr* handle_precrement(struct expr* lval, iopcode op) {
    assert(op == add || op == sub);
    check_lval(lval);
    check_arith(lval, "precrement");

    expr* res;

    if (lval->type == tableitem_e) {
        res = emit_iftableitem(lval);
        emit(op, res, newexpr_constint(1), res);
        emit(tablesetelem, lval->index, res, lval);
    } else {
        emit(op, lval, newexpr_constint(1), lval);
        res = newexpr(arithexpr_e);
        res->sym = newtemp_reuse(lval, NULL);
        emit(assign, lval, NULL, res);
    }
    return res;
}

expr* handle_postcrement(struct expr* lval, iopcode op) {
    assert(op == add || op == sub);
    check_lval(lval);
    check_arith(lval, "postcrement");

    expr* res;

    if (lval->type == tableitem_e) {
        res = emit_iftableitem(lval);
        emit(assign, res, NULL, res);
        emit(op, res, newexpr_constint(1), res);
        emit(tablesetelem, lval->index, res, lval);
    } else {
        res = newexpr(arithexpr_e);
        res->sym = newtemp_reuse(lval, NULL);
        emit(assign, lval, NULL, res);
        emit(op, lval, newexpr_constint(1), lval);
    }

    return res;
}

expr* handle_tablemake_elist(expr* elist) {
    expr* t = newexpr(newtable_e);
    t->sym = newtemp_reuse(NULL);
    emit(tablecreate, NULL, NULL, t);

    for (int i = 0; elist; elist = elist->next)
        emit(tablesetelem, newexpr_constint(i++), elist, t);

    return t;
}

expr* handle_tablemake_idx(expr* elist) {
    expr* t = newexpr(newtable_e);
    t->sym = newtemp_reuse(NULL);
    emit(tablecreate, NULL, NULL, t);

    for (; elist; elist = elist->next) {
        emit(tablesetelem, elist->index, elist, t);
    }

    return t;
}

struct stmt* handle_returnexpr(expr* retval) {
    printrule("returnexpr");

    if (currscopespace() != functionlocal) {
        fmt_error("Cannot return when not in function");
        // return NULL;
        exit(1);
    }

    // retval = short_circuit(retval);

    struct stmt* s = make_stmt();
    emit(ret, retval, NULL, NULL);
    emit_jump();
    s->returnlist = newlist(nextquadlabel() - 1);

    return s;
}
