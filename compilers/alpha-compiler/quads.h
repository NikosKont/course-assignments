#ifndef QUADS_H_
#define QUADS_H_

#define EXPAND_SIZE 1024
#define C_NUM_EXPAND_SZ 128
#define C_STR_EXPAND_SZ 128
#define C_LIBF_EXPAND_SZ 128
#define C_USERF_EXPAND_SZ 128
#define C_BOOL_EXPAND_SZ 128

#define CURR_SIZE (total * sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

extern void alpha_yyerror();
#define error alpha_yyerror

#include <assert.h>
#include <stdarg.h>

#include "datastructs.h"

extern char* opcode_str[];

typedef enum iopcode {
    assign,
    add,
    sub,
    mul,
    Div,
    mod,
    uminus,
    not,
    and,
    or,
    if_eq,
    if_noteq,
    if_lesseq,
    if_greatereq,
    if_less,
    if_greater,
    call,
    param,
    ret,
    getretval,
    funcstart,
    funcend,
    tablecreate,
    tablegetelem,
    tablesetelem,
    jump
} iopcode;

typedef enum expr_t {
    var_e,
    tableitem_e,

    programfunc_e,
    libraryfunc_e,

    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,

    constreal_e,
    constint_e,
    constbool_e,
    conststring_e,

    nil_e
} expr_t;

/* Scopespace and offsets */
typedef enum scopespace_t { programvar, functionlocal, formalarg } scopespace_t;

typedef enum symbol_t { var_s, programfunc_s, libfunc_s } symbol_t;

struct symbol {
    enum SymbolType type;
    char* name;
    scopespace_t space;
    unsigned offset;
    unsigned scope;
    unsigned line;
};

scopespace_t currscopespace();
unsigned currscopeoffset();
void increase_curr_offset();
void enter_scopespace();
void exit_scopespace();
int getcurline();
unsigned nextquadlabel();
void patchlabel(unsigned quadNo, unsigned label);

void resetfunctionlocalsoffset();

typedef struct logicList {
    int QuadNo;
    struct logicList* next;
} logicList_t;

typedef struct expr {
    expr_t type;
    SymTabEntry* sym;
    struct expr* index;
    int intConst;
    double realConst;
    char* strConst;  // needs allocation
    unsigned boolConst;
    struct expr* next;
    int truelist;
    int falselist;
} expr;

typedef struct quad {
    iopcode op;
    expr* result;
    expr* arg1;
    expr* arg2;
    unsigned label;
    unsigned line;
    unsigned taddress;
} quad;

struct call {
    expr* elist;
    char* name;
    int method;
};

int expr_to_bool(expr* e);
expr* expr_to_boolexpr(expr* e);

struct stmt {
    int breaklist, contlist, returnlist;
};

int newlist(int i);
int mergelist(int l1, int l2);
void patchlist(int list, int label);
void backpatch(logicList_t* list, int quadno);

void expand(void);

int emit_l(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned label);
int emit(iopcode op, expr* arg1, expr* arg2, expr* resulte);
int emit_jump();

expr* short_circuit(expr* e);

void print_quad(quad* q, FILE* fp);
void print_quad_idx(unsigned idx, FILE* fp);
void print_quads(FILE* fp);

void resetformalargsoffset();
void resetfunctionlocalsoffset();
void resettempfunc();
void restore_curr_scope_offset(unsigned);

expr* emit_iftableitem(expr* e);
expr* newexpr_conststring(char* s);
expr* newexpr_constint(int n);
expr* newexpr_constreal(double x);
expr* newexpr_constbool(unsigned b);
expr* newexpr(expr_t t);
expr* member_item(expr* lvalue, char* name);
expr* member_item_expr(expr* lvalue, expr* index);
void check_arith(expr* e, const char* context);

expr* reverse_elist(expr* elist);
expr* make_call(expr* lv, expr* elist);
expr* make_call_r(expr* lv, expr* reversed_elist);

struct forloopStruct {
    int test;
    int enter;
};

/*----------TEMP/HIDDEN VARIABLES----------*/

char* newtempname(void);
void resettemp(void);
SymTabEntry* newtemp(void);
SymTabEntry* newtemp_reuse(expr* e, ...);

unsigned istempname(char* s);
unsigned istempexpr(expr* e);

void fmt_error(char* msg, ...);

#endif
