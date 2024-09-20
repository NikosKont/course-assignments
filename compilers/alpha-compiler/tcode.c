#include "tcode.h"

#include "alphavm.h"
#include "quads.h"
extern quad* quads;
extern unsigned total;
extern unsigned currQuad;

const char* vmopcode_str[] = {
    "assign",    "add",      "sub",      "mul",          "div",
    "mod",       "uminus",   "and",      "or",           "not",
    "jeq",       "jne",      "jle",      "jge",          "jlt",
    "jgt",       "call",     "pusharg",  "ret",          "getretval",
    "funcenter", "funcexit", "newtable", "tablegetelem", "tablesetelem",
    "jump",      "nop"};

const char* vmarg_str[] = {"label",    "global",  "formal", "local",
                           "number",   "string",  "bool",   "nil",
                           "userfunc", "libfunc", "retval"};

instruction* instructions = (instruction*)0;
unsigned totalInstr = 0;
unsigned currInstr = 0;

unsigned currprocessedquad = 0;

unsigned nextInstructionLabel(void) { return currInstr; }

double* numConsts;
unsigned totalNumConsts = 0;
unsigned maxNumConsts = 0;

char** stringConsts;
unsigned totalStringConsts = 0;
unsigned maxStringConsts = 0;

// char** namedLibfuncs;
// #define namedLibfuncs lib_funcs
// extern char** namedLibfuncs;
const char* namedLibfuncs[] = {"print",
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
const unsigned totalNamedLibfuncs = 12;
const unsigned maxNamedLibfuncs = 12;

userfunc_t* userFuncs;
unsigned totalUserfuncs = 0;
unsigned maxUserfuncs = 0;

unsigned* boolConsts;
unsigned totalBoolConsts = 0;
unsigned maxBoolConsts = 0;

void expand_instr(void) {
    assert(totalInstr == currInstr);

    instruction* p = (instruction*)malloc(NEW_INSTRSIZE);

    if (instructions) {
        memcpy(p, instructions, CURR_SIZE);
        free(instructions);
    }
    instructions = p;
    totalInstr += EXPAND_SIZE;
    return;
}

void expand_numConsts(void) {
    assert(maxNumConsts == totalNumConsts);

    double* p = (double*)malloc(NEW_NUMCONSTSIZE);

    if (numConsts) {
        memcpy(p, numConsts, totalNumConsts * sizeof(double));
        free(numConsts);
    }
    numConsts = p;
    maxNumConsts += C_NUM_EXPAND_SZ;
    return;
}

void expand_stringConsts(void) {
    assert(maxStringConsts == totalStringConsts);

    char** p = (char**)malloc(NEW_STRCONSTSIZE);

    if (stringConsts) {
        memcpy(p, stringConsts, totalStringConsts * sizeof(char*));
        free(stringConsts);
    }
    stringConsts = p;
    maxStringConsts += C_STR_EXPAND_SZ;
    return;
}

void expand_userfuncConsts(void) {
    assert(maxUserfuncs == totalUserfuncs);

    userfunc_t* p = (userfunc_t*)malloc(NEW_USERFCONSTSIZE);

    if (userFuncs) {
        memcpy(p, userFuncs, totalUserfuncs * sizeof(userfunc_t));
        free(userFuncs);
    }
    userFuncs = p;
    maxUserfuncs += C_USERF_EXPAND_SZ;
    return;
}

void expand_boolConsts(void) {
    assert(maxBoolConsts == totalBoolConsts);

    unsigned* p = (unsigned*)malloc(NEW_BOOLCONSTSIZE);

    if (boolConsts) {
        memcpy(p, boolConsts, totalBoolConsts * sizeof(unsigned));
        free(boolConsts);
    }
    boolConsts = p;
    maxBoolConsts += C_BOOL_EXPAND_SZ;
    return;
}

unsigned consts_newstring(char* str) {
    if (totalStringConsts == maxStringConsts) expand_stringConsts();
    for (unsigned i = 0; i < totalStringConsts; i++) {
        if (stringConsts[i] == str) return i;
    }
    stringConsts[totalStringConsts] = str;

    return totalStringConsts++;
}

unsigned consts_newbool(unsigned val) {
    if (totalBoolConsts == maxBoolConsts) expand_boolConsts();
    for (unsigned i = 0; i < totalBoolConsts; i++) {
        if (boolConsts[i] == val) return i;
    }
    boolConsts[totalBoolConsts] = val;

    return totalBoolConsts++;
}

unsigned consts_newnumber(double num) {
    if (totalNumConsts == maxNumConsts) expand_numConsts();
    for (unsigned i = 0; i < totalNumConsts; i++) {
        if (numConsts[i] == num) return i;
    }
    numConsts[totalNumConsts] = num;

    return totalNumConsts++;
}

extern int search_array();
unsigned libfuncs_getindex(char* name) {
    // if (totalNamedLibfuncs == maxNamedLibfuncs) expand_libfuncConsts();

    for (unsigned i = 0; i < totalNamedLibfuncs; i++) {
        if (strcmp(namedLibfuncs[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

unsigned userfuncs_newfunc(SymTabEntry* sym) {
    if (totalUserfuncs == maxUserfuncs) expand_userfuncConsts();
    assert(sym && sym->type == USERFUNC_T);

    for (unsigned i = 0; i < totalUserfuncs; i++) {
        if (userFuncs[i].address == sym->value.funcVal->iaddress) return i;
    }
    userFuncs[totalUserfuncs].id = get_symbol_name(sym);
    userFuncs[totalUserfuncs].address =
        sym->value.funcVal
            ->iaddress;  // TODO MAY NEED +1/-1 INDEXING,SHOULD BE GOOD
    userFuncs[totalUserfuncs].localSize = sym->value.funcVal->total_locals;

    return totalUserfuncs++;
}

unsigned emit_instr(instruction* instr) {
    if (currInstr == totalInstr) expand_instr();
    if (totalNumConsts == maxNumConsts) expand_numConsts();
    if (totalStringConsts == maxStringConsts) expand_stringConsts();
    // if (totalNamedLibfuncs == maxNamedLibfuncs) expand_libfuncConsts();
    if (totalUserfuncs == maxUserfuncs) expand_userfuncConsts();
    if (totalBoolConsts == maxBoolConsts) expand_boolConsts();

    instruction* i = instructions + currInstr;

    i->opcode = instr->opcode;
    i->arg1 = instr->arg1;
    i->arg2 = instr->arg2;
    i->result = instr->result;
    i->srcLine = instr->srcLine;

    return currInstr++;
}

void backpatch_instr(returnList* returnlist, unsigned instrLabel) {
    assert(instrLabel < totalInstr);
    returnList* temp = returnlist;
    while (temp) {
        instructions[temp->instrLabel].result->val = instrLabel;
        temp = temp->next;
    }
    return;
}

void reset_operand(vmarg* arg) {
    if (!arg) return;
    arg->val = -1;
}

// TODO: might need to make numbers only REAL
vmarg* make_num_operand(double val) {
    vmarg* arg = (vmarg*)malloc(sizeof(vmarg));
    arg->val = consts_newnumber(val);
    arg->type = number_a;

    return arg;
}

vmarg* make_bool_operand(unsigned val) {
    vmarg* arg = (vmarg*)malloc(sizeof(vmarg));
    arg->val = consts_newbool(val);
    arg->type = bool_a;

    return arg;
}

vmarg* make_retval_operand() {
    vmarg* arg = (vmarg*)malloc(sizeof(vmarg));
    arg->type = retval_a;
    arg->val = -1;

    return arg;
}

vmarg* make_operand(expr* e) {
    if (!e) return NULL;

    vmarg* arg = (vmarg*)malloc(sizeof(vmarg));

    memset(arg, 0, sizeof(vmarg));

    fprintf(stdout, "MAKING OPERAND : %d\n", e->type);

    switch (e->type) {
        case assignexpr_e:
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e: {
            assert(e->sym);
            fprintf(stdout, "GETTING VAR TYPE:AND OFFSET: \n");
            arg->val = get_var_offset(e->sym);
            fprintf(stdout, "VAR TYPE: %d OFFSET: %d\n", arg->type, arg->val);

            switch (e->sym->value.varVal->space) {
                case programvar:
                    arg->type = global_a;
                    break;
                case functionlocal:
                    arg->type = local_a;
                    break;
                case formalarg:
                    arg->type = formal_a;
                    break;
                default:
                    assert(0);
            }
            break;
        }
        case constbool_e: {
            // arg->val = consts_newbool(e->boolConst);
            arg->val = e->boolConst;
            arg->type = bool_a;
            break;
        }
        case conststring_e: {
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;
            break;
        }
        // TODO: MAKE CONSTREAL AND CONSTINT INTO ONE CONSTNUM
        case constreal_e: {
            arg->val = consts_newnumber(e->realConst);
            arg->type = number_a;
            break;
        }
        case constint_e: {
            arg->val = consts_newnumber(e->intConst);
            fprintf(stdout, "GENERATING INT : \n");
            arg->type = number_a;
            break;
        }
        case nil_e: {
            arg->type = nil_a;
            break;
        }
        case programfunc_e: {
            // arg->val = e->sym->value.funcVal->iaddress;
            // TODO: OR ALTERNATIVELY:
            printf("NIGGGGAAAA\n");

            arg->val = userfuncs_newfunc(e->sym);
            arg->type = userfunc_a;
            break;
        }
        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_getindex(get_symbol_name(e->sym));
            break;
        }
        default:
            assert(0);
    }

    printf("MADE OPERAND: %d\n", arg->val);
    return arg;
}

void generate(vmopcode op, quad* q) {
    q->taddress = nextInstructionLabel();

    fprintf(stdout, "GENERATING CURR INSTR: %d\n", currInstr);

    fprintf(stdout, "CURR QUAD LABEL: %d\n", q->label);
    print_quad_idx(currprocessedquad, stdout);

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));

    t->opcode = op;
    t->srcLine = q->line;
    fprintf(stdout, "MAKE OPERAND ARG1: %d\n", currInstr);
    t->arg1 = make_operand(q->arg1);
    fprintf(stdout, "MAKE OPERAND ARG2: %d\n", currInstr);
    t->arg2 = make_operand(q->arg2);
    fprintf(stdout, "MAKE OPERAND RESULT: %d\n", currInstr);
    t->result = make_operand(q->result);

    fprintf(stdout, "CURRINSTR: %d\n", currInstr);
    emit_instr(t);
    fprintf(stdout, "CURRINSTR: %d\n", currInstr);

    return;
}

void generate_ASSIGN(quad* q) { generate(assign_v, q); }

void generate_ADD(quad* q) { generate(add_v, q); }
void generate_SUB(quad* q) { generate(sub_v, q); }
void generate_MUL(quad* q) { generate(mul_v, q); }
void generate_DIV(quad* q) { generate(div_v, q); }
void generate_MOD(quad* q) { generate(mod_v, q); }

void generate_UMINUS(quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = (instruction*)malloc(sizeof(instruction));
    t->opcode = mul_v;
    t->srcLine = q->line;
    t->arg1 = make_operand(q->arg1);
    t->arg2 = make_num_operand(-1);
    t->result = make_operand(q->result);

    emit_instr(t);

    return;
}

void generate_NOT(quad* q) { return; }
void generate_AND(quad* q) { return; }
void generate_OR(quad* q) { return; }

void generate_relational(vmopcode op, quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = (instruction*)malloc(sizeof(instruction));
    t->opcode = op;
    t->srcLine = q->line;
    t->arg1 = make_operand(q->arg1);
    t->arg2 = make_operand(q->arg2);
    t->result = malloc(sizeof(vmarg*));
    t->result->type = label_a;

    if (q->label < currprocessedquad) {
        t->result->val = quads[q->label - 1]
                             .taddress;  // TODO:CHECK INDEXING, SHOULD BE GOOD
    } else {
        add_incomplete_jump(nextInstructionLabel(), q->label - 1);
    }

    emit_instr(t);

    return;
}

void generate_IF_EQ(quad* q) { generate_relational(jeq_v, q); }
void generate_IF_NOTEQ(quad* q) { generate_relational(jne_v, q); }
void generate_IF_LESSEQ(quad* q) { generate_relational(jle_v, q); }
void generate_IF_GREATEREQ(quad* q) { generate_relational(jge_v, q); }
void generate_IF_LESS(quad* q) { generate_relational(jlt_v, q); }
void generate_IF_GREATER(quad* q) { generate_relational(jgt_v, q); }

// TODO: check usage
funcStack* funcStack_top = (funcStack*)0;

void funcStack_push(SymTabEntry* func) {
    assert(func);

    funcStack* new_func = (funcStack*)malloc(sizeof(funcStack));

    new_func->func = func;
    new_func->next = funcStack_top;

    funcStack_top = new_func;

    return;
}

SymTabEntry* funcStack_pop() {
    SymTabEntry* func = funcStack_top->func;
    funcStack_top = funcStack_top->next;
    return func;
}

void generate_CALL(quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));
    t->opcode = call_v;
    t->srcLine = q->line;
    t->arg1 = make_operand(q->arg1);

    emit_instr(t);
}

void generate_PARAM(quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));
    t->opcode = pusharg_v;
    t->srcLine = q->line;
    t->arg1 = make_operand(q->arg1);

    emit_instr(t);
}

void generate_RETURN(quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = malloc(sizeof(instruction));
    t->opcode = assign_v;
    t->srcLine = q->line;
    // t->arg1 = NULL;
    // t->arg2 = NULL;
    // t->result = NULL;

    t->result = make_retval_operand();
    if (q->arg1) {
        t->arg1 = make_operand(q->arg1);
    }
    emit_instr(t);

    return;
}

void generate_GETRETVAL(quad* q) {
    q->taddress = nextInstructionLabel();

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));
    t->opcode = assign_v;
    t->srcLine = q->line;
    t->result = make_operand(q->result);
    t->arg1 = make_retval_operand();

    emit_instr(t);
}

void generate_FUNCSTART(quad* q) {
    q->taddress = nextInstructionLabel();

    fprintf(stdout, "FUNCSTART\n");
    print_quad_idx(currprocessedquad, stdout);
    fprintf(stdout, "%d\n", q->arg1->type);

    SymTabEntry* f = q->arg1->sym;

    f->value.funcVal->taddress = nextInstructionLabel();

    // TODO: add to userfuncs (maybe)
    funcStack_push(f);

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));
    t->opcode = funcenter_v;
    t->srcLine = q->line;
    t->result = make_operand(q->arg1);
    emit_instr(t);
}

void generate_FUNCEND(quad* q) {
    q->taddress = nextInstructionLabel();

    SymTabEntry* f = funcStack_pop();
    backpatch_instr(f->value.funcVal->returnlist, nextInstructionLabel());

    instruction* t = (instruction*)malloc(sizeof(instruction));
    memset(t, 0, sizeof(instruction));
    t->opcode = funcexit_v;
    t->result = make_operand(q->arg1);
    emit_instr(t);

    return;
}

void generate_NEWTABLE(quad* q) {
    printf("genNEWTABLE\n");
    generate(newtable_v, q);
}
void generate_TABLEGETELEM(quad* q) { generate(tablegetelem_v, q); }
void generate_TABLESETELEM(quad* q) {
    printf("genseTELEM\n");
    generate(tablesetelem_v, q);
}

void generate_JUMP(quad* q) { generate_relational(jump_v, q); }

void generate_NOP() {
    quad* q = (quad*)malloc(sizeof(quad));
    memset(q, 0, sizeof(quad));
    generate(nop_v, q);
}

incomplete_jump* ij_head = (incomplete_jump*)0;
unsigned ij_total = 0;

void add_incomplete_jump(unsigned instrNo, unsigned iaddress) {
    incomplete_jump* inc_jump =
        (incomplete_jump*)malloc(sizeof(incomplete_jump));
    inc_jump->instrNo = instrNo;
    inc_jump->iaddress = iaddress;
    inc_jump->next = ij_head;
    ij_head = inc_jump;
    ij_total++;
    return;
}

void patch_incomplete_jumps(void) {
    // TODO:LECTURE 14 SLIDE 15
    incomplete_jump* temp = ij_head;
    while (temp) {
        if (temp->iaddress == nextquadlabel())  // MAYBE NEXT QUAD LABEL-1
        {
            instructions[temp->instrNo].result->val = currInstr;
        } else {
            instructions[temp->instrNo].result->val =
                quads[temp->iaddress].taddress;
        }
        temp = temp->next;
    }
    return;
}

generator_func_t generators[] = {
    generate_ASSIGN,       generate_ADD,          generate_SUB,
    generate_MUL,          generate_DIV,          generate_MOD,
    generate_UMINUS,       generate_AND,          generate_OR,
    generate_NOT,          generate_IF_EQ,        generate_IF_NOTEQ,
    generate_IF_LESSEQ,    generate_IF_GREATEREQ, generate_IF_LESS,
    generate_IF_GREATER,   generate_CALL,         generate_PARAM,
    generate_RETURN,       generate_GETRETVAL,    generate_FUNCSTART,
    generate_FUNCEND,      generate_NEWTABLE,     generate_TABLEGETELEM,
    generate_TABLESETELEM, generate_JUMP,         generate_NOP};

void generate_target_code() {
    for (unsigned i = 0; i < currQuad; i++) {
        fprintf(stdout, "CRAZY\n");
        currprocessedquad = i;
        fprintf(stdout, "QUAD OPCODE: %d\n", quads[i].op);
        (*generators[quads[i].op])(&quads[i]);
    }

    patch_incomplete_jumps();
}

char* vmarg_content_string(vmarg* a) {
    vmarg_t type = a->type;
    char string[1024];

    sprintf(string, "%d (%s), ", a->type, vmarg_str[type]);

    sprintf(string + strlen(string), "%d", a->val);

    return strdup(string);
}

void print_constArrays(void) {
    // FILE *fp = stdout;
    FILE* consts = fopen("consts.txt", "w");
    // Print numConsts
    fprintf(consts, "Number Constants:\n");
    for (unsigned i = 0; i < totalNumConsts; ++i) {
        fprintf(consts, "%f\n", numConsts[i]);
    }
    fprintf(consts, "\n");

    // Print stringConsts
    fprintf(consts, "String Constants:\n");
    for (unsigned i = 0; i < totalStringConsts; ++i) {
        fprintf(consts, "%s\n", stringConsts[i]);
    }
    fprintf(consts, "\n");

    // Print namedLibfuncs
    fprintf(consts, "Named Library Functions:\n");
    for (unsigned i = 0; i < totalNamedLibfuncs; ++i) {
        fprintf(consts, "%s\n", namedLibfuncs[i]);
    }
    fprintf(consts, "\n");

    // Print userFuncs
    fprintf(consts, "User Functions:\n");
    for (unsigned i = 0; i < totalUserfuncs; ++i) {
        fprintf(consts, "User Function Id: %s, Address: %d\n", userFuncs[i].id,
                userFuncs[i].address);
    }
    fprintf(consts, "\n");

    // Print boolConsts
    fprintf(consts, "Boolean Constants:\n");
    for (unsigned i = 0; i < totalBoolConsts; ++i) {
        fprintf(consts, "%u\n", boolConsts[i]);
    }
    fprintf(consts, "\n");

    fclose(consts);
}

void print_instruction(instruction* instr, FILE* fp) {
    fprintf(fp, "%s ", vmopcode_str[instr->opcode]);

    if (instr->result)
        fprintf(fp, "res: %s ", vmarg_content_string(instr->result));

    if (instr->arg1)
        fprintf(fp, "arg1: %s ", vmarg_content_string(instr->arg1));

    if (instr->arg2)
        fprintf(fp, "arg2: %s ", vmarg_content_string(instr->arg2));

    fprintf(fp, "[line %u]\n", instr->srcLine);
}
void print_instruction_idx(unsigned idx, FILE* fp) {
    assert(idx >= 0 && idx <= currInstr);
    print_instruction(instructions + idx, fp);
}
void print_instructions(FILE* fp) {
    fprintf(stdout, "CRAZY\n");

    if (!fp) fp = fopen("tcode.txt", "w");
    if (!fp) fp = stdout;

    for (unsigned i = 0; i < currInstr; ++i) {
        fprintf(fp, "%d: ", i);
        print_instruction_idx(i, fp);
    }

    print_constArrays();
}

extern unsigned programVarOffset;

void createbin(void) {
    unsigned magic = 340;
    unsigned long str_len;
    FILE* bytefile = fopen("alpha.out", "w");
    ;
    size_t i;

    // magic number
    fwrite(&magic, sizeof(unsigned), 1, bytefile);

    // global offset
    fwrite(&programVarOffset, sizeof(unsigned), 1, bytefile);

    // numerical consts
    unsigned numConstsSize = totalNumConsts;
    fwrite(&numConstsSize, sizeof(unsigned), 1, bytefile);
    for (i = 0; i < numConstsSize; i++) {
        fwrite(&numConsts[i], sizeof(double), 1, bytefile);
    }

    // string consts
    unsigned strConstsSize = totalStringConsts;
    fwrite(&strConstsSize, sizeof(unsigned), 1, bytefile);
    for (i = 0; i < strConstsSize; i++) {
        str_len = strlen(stringConsts[i]);
        fwrite(&str_len, sizeof(unsigned long), 1, bytefile);
        fwrite(strdup(stringConsts[i]), sizeof(char), str_len + 1, bytefile);
    }

    // bool consts
    unsigned boolConstsSize = totalBoolConsts;
    fwrite(&boolConstsSize, sizeof(unsigned), 1, bytefile);
    for (i = 0; i < boolConstsSize; i++) {
        unsigned boolAsInt = boolConsts[i] ? 1 : 0;
        fwrite(&boolAsInt, sizeof(unsigned), 1, bytefile);
    }

    // Library functions
    unsigned libFuncsSize = totalNamedLibfuncs;
    fwrite(&libFuncsSize, sizeof(unsigned), 1, bytefile);
    for (i = 0; i < libFuncsSize; i++) {
        str_len = strlen(namedLibfuncs[i]);
        fwrite(&str_len, sizeof(unsigned long), 1, bytefile);
        fwrite(strdup(namedLibfuncs[i]), sizeof(char), str_len + 1, bytefile);
    }

    // User functions
    unsigned usrFuncsSize = totalUserfuncs;
    fwrite(&usrFuncsSize, sizeof(unsigned), 1, bytefile);
    for (i = 0; i < usrFuncsSize; i++) {
        fwrite(&userFuncs[i].address, sizeof(unsigned), 1, bytefile);
        fwrite(&userFuncs[i].localSize, sizeof(unsigned), 1, bytefile);

        str_len = strlen(userFuncs[i].id);
        fwrite(&str_len, sizeof(unsigned long), 1, bytefile);
        fwrite(strdup(userFuncs[i].id), sizeof(char), str_len + 1, bytefile);
    }

    // instructions
    unsigned instrSize = totalInstr;
    fwrite(&instrSize, sizeof(unsigned), 1, bytefile);
    for (i = 1; i < instrSize; i++) {
        fwrite(&(instructions[i].opcode), sizeof(vmopcode), 1, bytefile);
        // result
        unsigned isResultNull =
            !instructions[i].result;  // all commands have a res
        fwrite(&isResultNull, sizeof(unsigned), 1, bytefile);

        if (!isResultNull) {
            fwrite(&(instructions[i].result->type), sizeof(vmarg_t), 1,
                   bytefile);
            fwrite(&(instructions[i].result->val), sizeof(unsigned), 1,
                   bytefile);
        }

        unsigned isArg1Null = !instructions[i].arg1;
        fwrite(&isArg1Null, sizeof(unsigned), 1, bytefile);
        if (!isArg1Null) {
            fwrite(&(instructions[i].arg1->type), sizeof(vmarg_t), 1, bytefile);
            fwrite(&(instructions[i].arg1->val), sizeof(unsigned), 1, bytefile);
        }

        unsigned isArg2Null = !instructions[i].arg2;
        fwrite(&isArg2Null, sizeof(unsigned), 1, bytefile);
        if (!isArg2Null) {
            fwrite(&(instructions[i].arg2->type), sizeof(vmarg_t), 1, bytefile);
            fwrite(&(instructions[i].arg2->val), sizeof(unsigned), 1, bytefile);
        }

        fwrite(&(instructions[i].srcLine), sizeof(unsigned), 1, bytefile);
    }

    fclose(bytefile);
}
