#ifndef ALPHAVM_H_
#define ALPHAVM_H_
#include <math.h>
#include <stdlib.h>

#include "assert.h"
#include "tcode.h"

#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_MAX_INSTRUCTIONS nop_v
#define MAGIC_NUMBER 340
#define HASH 5381

#define AVM_STACKENV_SIZE 4
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#define execute_jle execute_comparison
#define execute_jge execute_comparison
#define execute_jgt execute_comparison
#define execute_jlt execute_comparison

enum avm_memcell_t {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
};

typedef struct avm_table avm_table;

typedef struct avm_memcell {
    enum avm_memcell_t type;
    union {
        double numVal;
        char* strVal;
        unsigned boolVal;
        avm_table* tableVal;
        unsigned funcVal;
        unsigned libfuncVal;
        // char* libfuncVal;
    } data;
} avm_memcell;

#define AVM_TABLE_HASHSIZE 211

typedef struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    struct avm_table_bucket* next;
} avm_table_bucket;

#define INDEXED_TYPES 6

// struct table_indexed {
//     avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
//     avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
//     avm_table_bucket* boolIndexed[2];
//     avm_table_bucket* tableIndexed[AVM_TABLE_HASHSIZE];
//     avm_table_bucket* userfuncIndexed[AVM_TABLE_HASHSIZE];
//     avm_table_bucket* libfuncIndexed[AVM_TABLE_HASHSIZE];
// };

struct avm_table {
    unsigned refCounter;
    avm_table_bucket*** indexed;
    unsigned total;
};

typedef void (*execute_func_t)(instruction*);
typedef void (*memclear_func_t)(avm_memcell*);
typedef void (*libraryfunc_t)(void);
typedef char* (*tostring_func_t)(avm_memcell*);
typedef double (*arithmetic_func_t)(double x, double y);
typedef unsigned char (*comp_func_t)(double x, double y);
typedef unsigned char (*tobool_func_t)(avm_memcell*);
typedef unsigned char (*eq_func_t)(avm_memcell*, avm_memcell*);

char* number_tostring(avm_memcell* m);
char* string_tostring(avm_memcell* m);
char* bool_tostring(avm_memcell* m);
char* table_tostring(avm_memcell* m);
char* userfunc_tostring(avm_memcell* m);
char* libfunc_tostring(avm_memcell* m);
char* nil_tostring(avm_memcell* m);
char* undef_tostring(avm_memcell* m);

char* avm_tostring(avm_memcell* m);

double consts_getnumber(unsigned index);
char* consts_getstring(unsigned index);
char* libfuncs_getused(unsigned index);
userfunc_t* userfuncs_getfunc(unsigned address);
userfunc_t* avm_getfuncinfo(unsigned address);

void execute_assign(instruction* instr);

void execute_arithmetic(instruction* instr);
void execute_add(instruction* instr);
void execute_sub(instruction* instr);
void execute_mul(instruction* instr);
void execute_div(instruction* instr);
void execute_mod(instruction* instr);
void execute_uminus(instruction* instr);

void execute_not(instruction* instr);
void execute_and(instruction* instr);
void execute_or(instruction* instr);

void execute_comparison(instruction* instr);
void execute_jeq(instruction* instr);
void execute_jne(instruction* instr);
void execute_jle(instruction* instr);
void execute_jge(instruction* instr);
void execute_jlt(instruction* instr);
void execute_jgt(instruction* instr);

void execute_call(instruction* instr);
void execute_pusharg(instruction* instr);
void execute_ret(instruction* instr);
void execute_getretval(instruction* instr);
void execute_funcenter(instruction* instr);
void execute_funcexit(instruction* instr);

void execute_newtable(instruction* instr);
void execute_tablegetelem(instruction* instr);
void execute_tablesetelem(instruction* instr);

void execute_jump(instruction* instr);
void execute_nop(instruction* instr);

void avm_callLibFunc_idx(unsigned idx);
void avm_callLibFunc_name(char* id);
libraryfunc_t avm_getlibraryfunc(char* id);
void libfunc_print();
void libfunc_input();
void libfunc_objectmemberkeys();
void libfunc_objecttotalmembers();
void libfunc_objectcopy();
void libfunc_totalarguments();
void libfunc_argument();
void libfunc_typeof();
void libfunc_strtonum();
void libfunc_sqrt();
void libfunc_cos();
void libfunc_sin();

avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg);

void memclear_string(avm_memcell* m);
void memclear_table(avm_memcell* m);
void avm_memcellclear(avm_memcell* m);

void avm_initstack();
void avm_dec_top();
void avm_push_envvalue(unsigned val);
void avm_callSaveEnv();
unsigned avm_get_envvalue(unsigned i);

unsigned avm_totalactuals();
avm_memcell* avm_getactual(unsigned i);

unsigned char avm_tobool(avm_memcell* m);
unsigned char number_eq(avm_memcell* rv1, avm_memcell* rv2);
unsigned char string_eq(avm_memcell* rv1, avm_memcell* rv2);
unsigned char bool_eq(avm_memcell* rv1, avm_memcell* rv2);

void avm_assign(avm_memcell* lv, avm_memcell* rv);

void avm_call_functor(avm_table* t);
void avm_push_table_arg(avm_table* t);
void avm_tableincrefcounter(avm_table* t);
void avm_tabledecrefcounter(avm_table* t);
void avm_tablebucketsinit(avm_table_bucket** p);
avm_table* avm_tablenew();
avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);
void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* value);
void avm_tablebuckets_destroy(avm_table_bucket** p);
void avm_tabledestroy(avm_table* t);

void execute_cycle();

void* avm_error(char* msg, ...);
void avm_warning(char* msg, ...);

void avm_initialize();

#endif