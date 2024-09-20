#ifndef TCODE_H_
#define TCODE_H_

#include "datastructs.h"
#include "quads.h"

typedef enum vmopcode {
    assign_v,
    add_v,
    sub_v,
    mul_v,
    div_v,
    mod_v,
    uminus_v,
    not_v,
    and_v,
    or_v,
    jeq_v,
    jne_v,
    jle_v,
    jge_v,
    jlt_v,
    jgt_v,
    call_v,
    pusharg_v,
    ret_v,
    getretval_v,
    funcenter_v,
    funcexit_v,
    newtable_v,
    tablegetelem_v,
    tablesetelem_v,
    jump_v,
    nop_v
} vmopcode;

typedef enum vmarg_t {
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a
} vmarg_t;

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
} vmarg;

typedef struct instruction {
    vmopcode opcode;
    vmarg* result;
    vmarg* arg1;
    vmarg* arg2;
    unsigned srcLine;
} instruction;

#define CURR_INSTRSIZE (totalInstr * sizeof(instruction))
#define CURR_NUMSIZE (totalInstr * sizeof(double))
#define CURR_STRSIZE (totalInstr * sizeof(char*))
#define CURR_LIBFSIZE (totalInstr * sizeof(char*))
#define CURR_USERFSIZE (totalInstr * sizeof(userfunc_t))
#define CURR_BOOLSIZE (totalInstr * sizeof(unsigned))

#define NEW_INSTRSIZE (EXPAND_SIZE * sizeof(instruction) + CURR_INSTRSIZE)

#define NEW_NUMCONSTSIZE (C_NUM_EXPAND_SZ * sizeof(double) + CURR_NUMSIZE)
#define NEW_STRCONSTSIZE (C_STR_EXPAND_SZ * sizeof(char*) + CURR_STRSIZE)
#define NEW_LIBFCONSTSIZE (C_LIBF_EXPAND_SZ * sizeof(char*) + CURR_LIBFSIZE)
#define NEW_USERFCONSTSIZE \
    (C_USERF_EXPAND_SZ * sizeof(userfunc_t) + CURR_USERFSIZE)
#define NEW_BOOLCONSTSIZE (C_BOOL_EXPAND_SZ * sizeof(unsigned) + CURR_BOOLSIZE)

typedef struct userfunc {
    unsigned address;
    unsigned localSize;
    char* id;
} userfunc_t;

//          .c
// double*     numConsts;
// unsigned    totalNumConsts;
// char**      stringConsts;
// unsigned    totalStringConsts;
// char**      namedLibfuncs;
// unsigned    totalNamedLibfuncs;
// userfunc_t*   userFuncs;
// unsigned    totalUserfuncs;

// TODO: might need to make numbers only REAL
vmarg* make_int_operand(int val);
vmarg* make_real_operand(double val);
vmarg* make_bool_operand(unsigned val);
vmarg* make_retval_operand();

unsigned libfuncs_getindex(char* name);

typedef struct incomplete_jump {
    unsigned instrNo;
    unsigned iaddress;
    struct incomplete_jump* next;
} incomplete_jump;

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
void patch_incomplete_jumps();

typedef struct funcStack {
    SymTabEntry* func;
    struct funcStack* next;
} funcStack;

typedef void (*generator_func_t)(quad*);

void generate_target_code();

void print_instruction(instruction* instr, FILE* fp);
void print_instruction_idx(unsigned idx, FILE* fp);
void print_instructions(FILE* fp);

void createbin(void);

#endif