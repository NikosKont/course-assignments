#include "quads.h"

extern int scope;
extern int alpha_yylineno;
extern expr* lvalue_expr(struct SymbolTableEntry* sym);

unsigned total = 1;
unsigned currQuad = 1;
quad* quads = (quad*)0;

unsigned programVarOffset = 0;
unsigned functionLocalOffset = 0;
unsigned formalArgOffset = 0;
unsigned scopeSpaceCounter = 1;

char* opcode_str[] = {
    "assign",     "add",      "sub",         "mul",          "div",
    "mod",        "uminus",   "not",         "and",          "or",
    "if_eq",      "if_noteq", "if_lesseq",   "if_greatereq", "if_less",
    "if_greater", "call",     "param",       "return",       "getretval",
    "funcstart",  "funcend",  "tablecreate", "tablegetelem", "tablesetelem",
    "jump"};

int getcurline() { return alpha_yylineno; }

void fmt_error(char* msg, ...) {
    va_list valist;
    va_start(valist, msg);
    char* new_msg = malloc(strlen(msg) + 100);

    vsprintf(new_msg, msg, valist);

    error(new_msg);

    va_end(valist);

    return;
}

expr* newexpr(expr_t t) {
    expr* e = (expr*)malloc(sizeof(expr));
    memset(e, 0, sizeof(expr));
    e->type = t;
    e->truelist = 0;
    e->falselist = 0;
    return e;
}
expr* newexpr_conststring(char* s) {
    expr* e = newexpr(conststring_e);
    e->strConst = strdup(s);
    return e;
}
expr* newexpr_constint(int n) {
    expr* e = newexpr(constint_e);
    e->intConst = n;
    return e;
}
expr* newexpr_constreal(double x) {
    expr* e = newexpr(constreal_e);
    e->realConst = x;
    return e;
}
expr* newexpr_constbool(unsigned b) {
    expr* e = newexpr(constbool_e);
    e->boolConst = !!b;
    return e;
}

unsigned istempname(char* s) { return *s == '$'; }

unsigned istempexpr(expr* e) {
    return e->sym && istempname(get_symbol_name(e->sym));
}

expr* make_call_r(expr* lv, expr* reversed_elist) {
    expr* func = emit_iftableitem(lv);
    while (reversed_elist) {
        emit(param, reversed_elist, NULL, NULL);
        reversed_elist = reversed_elist->next;
    }

    emit(call, func, NULL, NULL);
    expr* result = newexpr(var_e);
    result->sym = newtemp_reuse(lv, reversed_elist, NULL);
    emit(getretval, NULL, NULL, result);
    return result;
}
expr* make_call(expr* lv, expr* elist) {
    return make_call_r(lv, reverse_elist(elist));
}
expr* reverse_elist(expr* elist) {
    expr *p = NULL, *c = elist, *n = NULL;

    while (c) {
        n = c->next;
        c->next = p;
        p = c;
        c = n;
    }
    elist = p;
    return p;
}

int newlist(int i) {
    quads[i].label = 0;
    return i;
}

int mergelist(int l1, int l2) {
    if (!l1) return l2;
    if (!l2) return l1;

    int i = l1;
    while (quads[i].label) i = quads[i].label;

    quads[i].label = l2;

    return l1;
}

void patchlist(int list, int label) {
    while (list) {
        int next = quads[list].label;
        quads[list].label = label;
        list = next;
    }
}

void backpatch(logicList_t* list, int quadno) {
    logicList_t* tmp = list;

    while (tmp != NULL) {
        patchlabel(tmp->QuadNo, quadno);
        tmp = tmp->next;
    }
    return;
}

expr* short_circuit(expr* e) {
    if (!e) return e;
    // if (e->type == boolexpr_e || e->type == constbool_e) {
    if (e->type == boolexpr_e) {
        patchlist(e->truelist, nextquadlabel());
        patchlist(e->falselist, nextquadlabel() + 2);

        SymTabEntry* s = newtemp_reuse(e, NULL);
        e = newexpr(boolexpr_e);
        e->sym = s;

        emit(assign, newexpr_constbool(1), NULL, e);
        emit_l(jump, NULL, NULL, NULL, nextquadlabel() + 2);
        emit(assign, newexpr_constbool(0), NULL, e);
    }
    return e;
}

expr* member_item(expr* lvalue, char* name) {
    lvalue = emit_iftableitem(lvalue);
    expr* item = newexpr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = newexpr_conststring(name);
    return item;
}
expr* member_item_expr(expr* lvalue, expr* index) {
    lvalue = emit_iftableitem(lvalue);
    expr* item = newexpr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = index;
    return item;
}

void expand(void) {
    assert(total == currQuad);
    quad* p = (quad*)malloc(NEW_SIZE);
    if (quads) {
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
    return;
}

int emit_l(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned label) {
    // if (result) assert(result->type == var_e);

    if (currQuad == total) expand();

    quad* p = quads + currQuad;
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = getcurline();

    return currQuad++;
}
int emit(iopcode op, expr* arg1, expr* arg2, expr* result) {
    return emit_l(op, arg1, arg2, result, -1);
}
int emit_jump() { return emit_l(jump, NULL, NULL, NULL, 0); }

expr* emit_iftableitem(expr* e) {
    if (e->type != tableitem_e) return e;

    expr* result = lvalue_expr(newtemp_reuse(NULL));
    emit(tablegetelem, e, e->index, result);
    return result;
}

// TODO: complete
int expr_to_bool(expr* e) {
    switch (e->type) {
        case constbool_e:
            return e->boolConst;
        case conststring_e:
            return strcmp(e->strConst, "") != 0;
        case constint_e:
            return e->intConst != 0;
        case constreal_e:
            return e->realConst != 0.0;
        case nil_e:
            return 0;
        default:
            return 1;
    }
}
expr* expr_to_boolexpr(expr* e) {
    if (e->type == boolexpr_e) return e;

    if (e->type != constbool_e) {
        // e->type = boolexpr_e;
        // return e;
        e->type = boolexpr_e;
    }

    // int b = expr_to_bool(e);
    int b = 1;

    emit_l(if_eq, e, newexpr_constbool(b), NULL, 0);
    emit_jump();

    e->truelist = newlist(nextquadlabel() - 2);
    e->falselist = newlist(nextquadlabel() - 1);
    return e;
}

/*----------TEMP/HIDDEN VARIABLES----------*/

/*Scope set and offsets*/

alpha_stack_t* offsetStack = NULL;

scopespace_t currscopespace() {
    if (scopeSpaceCounter == 1)
        return programvar;
    else if (scopeSpaceCounter % 2 == 0)
        return formalarg;
    else
        return functionlocal;
}

unsigned currscopeoffset() {
    switch (currscopespace()) {
        case programvar:
            return programVarOffset;
        case functionlocal:
            return functionLocalOffset;
        case formalarg:
            return formalArgOffset;
        default:
            assert(0);
    }
}

void increase_curr_offset() {
    switch (currscopespace()) {
        case programvar:
            ++programVarOffset;
            break;
        case functionlocal:
            ++functionLocalOffset;
            break;
        case formalarg:
            ++formalArgOffset;
            break;
        default:
            assert(0);
    }
}

void resetfunctionlocalsoffset() { functionLocalOffset = 0; }

void resetformalargsoffset() { formalArgOffset = 0; }

void restore_curr_scope_offset(unsigned n) {
    switch (currscopespace()) {
        case programvar:
            programVarOffset = n;
            break;
        case functionlocal:
            functionLocalOffset = n;
            break;
        case formalarg:
            formalArgOffset = n;
            break;
        default:
            assert(0);
    }
    return;
}

unsigned nextquadlabel(void) { return currQuad; }

void patchlabel(unsigned quadNo, unsigned label) {
    assert(quadNo < currQuad);
    quads[quadNo].label = label;
}

void enter_scopespace() { ++scopeSpaceCounter; }

void exit_scopespace() {
    assert(scopeSpaceCounter > 1);
    --scopeSpaceCounter;
}

char* const_to_string(expr* e) {
    expr_t type = e->type;
    char string[1024];

    switch (type) {
        case constreal_e:
            sprintf(string, "%f", e->realConst);
            break;
        case constint_e:
            sprintf(string, "%d", e->intConst);
            break;
        case constbool_e:
            if (e->boolConst)
                sprintf(string, "%s", "true");
            else
                sprintf(string, "%s", "false");
            break;
        case conststring_e:
            sprintf(string, "%s", e->strConst);
            break;
        case nil_e:
            sprintf(string, "%s", "NULL");
            break;
            // default:
            //     assert(0);  // isn't const
    }

    return strdup(string);
}

// TODO: check if correct
char* expr_content_string(expr* e) {
    expr_t type = e->type;
    char string[1024];

    if (type == conststring_e) {
        strcpy(string, "\"");
        strcat(string, const_to_string(e));
        strcat(string, "\"");
    } else if (type == constbool_e || (type == boolexpr_e && e->sym == NULL)) {
        type = constbool_e;
        strcpy(string, "\'");
        strcat(string, const_to_string(e));
        strcat(string, "\'");
    } else if (type >= constreal_e)
        strcpy(string, const_to_string(e));
    else
        sprintf(string, "%s", get_symbol_name(e->sym));

    return strdup(string);
}

void check_arith(expr* e, const char* context) {
    if (e->type == constbool_e || e->type == conststring_e ||
        e->type == nil_e || e->type == newtable_e || e->type == programfunc_e ||
        e->type == libraryfunc_e || e->type == boolexpr_e) {
        fmt_error("Illegal expr used in %s!", context);
    }
}
void print_quad(quad* q, FILE* fp) {
    if (!q) return;
    if (!fp) return;

    fprintf(fp, "%s ", opcode_str[q->op]);

    // fprintf(fp, "(res) ");
    if (q->result) fprintf(fp, "%s ", expr_content_string(q->result));

    // fprintf(fp, "(arg1) ");
    if (q->arg1) fprintf(fp, "%s ", expr_content_string(q->arg1));

    // fprintf(fp, "(arg2) ");
    if (q->arg2) fprintf(fp, "%s ", expr_content_string(q->arg2));

    if ((q->op >= if_eq && q->op <= if_greater) || q->op == jump)
        // fprintf(fp, "(label) ");
        fprintf(fp, "%d ", q->label);

    fprintf(fp, "[line %d]\n", q->line);
}
void print_quad_idx(unsigned idx, FILE* fp) {
    if (idx < 1 || idx >= currQuad) return;
    print_quad(quads + idx, fp);
}
void print_quads(FILE* fp) {
    if (!fp) fp = fopen("interm_code.txt", "w");

    if (!fp) return;
    for (int i = 1; i < currQuad; i++) {
        fprintf(fp, "%d: ", i);
        print_quad(quads + i, fp);
    }

    // fclose(fp);
}