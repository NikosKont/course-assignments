%{
    #include <math.h>
    #include "parser_handlers.c"
    #include "alphavm.h"

    extern FILE* alpha_yyin;
    extern FILE* alpha_yyout;
    extern char* alpha_yytext;

    extern alpha_stack_t* offsetStack;

    extern unsigned executionFinished;

    FILE* quadout;
    FILE* instr_out;
    FILE* out;

    int alpha_yylex();
    void alpha_yyerror();
    void printrule(const char* rule);

    int scope = 0;
    int inFunction = 0;

%}

%name-prefix="alpha_yy"
%output="parser.c"
%error-verbose

%union{
    char* strVal;
    int intVal;
    unsigned int uintVal;
    double realVal;
    struct SymbolTableEntry* symVal;
    struct expr* exprVal;
    struct VariableList* varlistVal;
    struct Function *funcVal;
    struct call *callVal;
    struct forloopStruct* forloopVal;
    struct stmt* stmtVal;
}

%start program

// keywords
%token<strVal>  IF ELSE WHILE FOR FUNCTION BREAK CONT RETURN AND NOT OR LOCAL TRUE FALSE NIL

// operators
%token<strVal>  EQUAL PLUS MINUS STAR SLASH PERC EQ_EQ NEQ PLUS_PLUS MINUS_MINUS GREATER LESS GREQ LEQ

// punctuation
%token<strVal>  BRACE_L BRACE_R BRACKET_L BRACKET_R PARENTH_L PARENTH_R SEMICOLON COMMA COLON DOUBLE_COLON DOT DOT_DOT

%token<intVal>  INT
%token<realVal> REAL
%token<strVal>  ID
%token<strVal>  STRING


%right EQUAL
%left OR
%left AND
%nonassoc NEQ EQ_EQ
%nonassoc GREATER GREQ LESS LEQ
%left PLUS MINUS
%left STAR SLASH PERC
%right NOT PLUS_PLUS MINUS_MINUS UMINUS
%left DOT DOT_DOT
%left BRACKET_L BRACKET_R
%left PARENTH_L PARENTH_R


%type<exprVal>      lvalue member primary assignexpr call term tablemake const
%type<exprVal>      expr arithmexpr relexpr number boolexpr
%type<exprVal>      elist indexed indexedelem
%type<varlistVal>   idlist funcargs

%type<uintVal>      N M funcbody whilestart whilecond ifprefix elseprefix
%type<forloopVal>   forprefix

%type<strVal>       funcname
%type<symVal>       funcdef funcprefix

%type<callVal>      callsuffix normcall methodcall

%type<stmtVal>      stmtlist stmt breakstmt continuestmt loopstmt
%type<stmtVal>      ifstmt returnstmt forstmt whilestmt block


%%


program:    stmtlist        {printrule("program -> stmtlist");}
            | /*empty*/     {printrule("program -> ε");}
            ;

stmtlist:   stmtlist stmt
            {
                printrule("stmtlist -> stmtlist stmt");
                $$ = $1;
                $$->breaklist = mergelist($1->breaklist, $2->breaklist);
                $$->contlist = mergelist($1->contlist, $2->contlist);
                $$->returnlist = mergelist($1->returnlist, $2->returnlist);
            }
            |stmt   {printrule("stmtlist -> stmt");$$ = $1;}
            ;

stmt:   expr SEMICOLON      {$$ = make_stmt();$1 = short_circuit($1);resettempcounter();printrule("stmt -> expr;");}
        |ifstmt             {$$ = $1;resettempcounter();printrule("stmt -> if");}
        |whilestmt          {$$ = make_stmt();resettempcounter();printrule("stmt -> while");}
        |forstmt            {$$ = make_stmt();resettempcounter();printrule("stmt -> for");}
        |returnstmt         {$$ = $1;printrule("stmt -> return");}
        |breakstmt          {$$ = $1;resettempcounter();printrule("stmt -> break");}
        |continuestmt       {$$ = $1;resettempcounter();printrule("stmt -> continue");}
        |block              {$$ = $1;resettempcounter();printrule("stmt -> block");}
        |funcdef            {$$ = make_stmt();resettempcounter();printrule("stmt -> funcdef");}
        |SEMICOLON          {$$ = make_stmt();resettempcounter();printrule("stmt -> empty");}
        ;

breakstmt:  BREAK SEMICOLON
            {
                printrule("breakstmt");
                if(loopcounter <= 0 ) {
                    fmt_error("break statement outside of loop");
                }

                $$ = make_stmt();
                $$->breaklist = newlist(nextquadlabel());
                emit_jump();
            }
            ;

continuestmt:   CONT SEMICOLON
                {
                    printrule("continuestmt");
                    if(loopcounter <= 0 ) {
                        fmt_error("continue statement outside of loop");
                    }

                    $$ = make_stmt();
                    $$->contlist = newlist(nextquadlabel());
                    emit_jump();
                }
                ;

returnstmt: RETURN expr SEMICOLON   {$$ = handle_returnexpr($2);}
            |RETURN SEMICOLON       {$$ = handle_returnexpr(NULL);}
            ;

assignexpr:     lvalue EQUAL expr   {$$ = handle_assign($1, $3);}

arithmexpr:     expr PLUS expr      {$$ = handle_arithmexpr($1, add, $3);}
                |expr MINUS expr    {$$ = handle_arithmexpr($1, sub, $3);}
                |expr STAR expr     {$$ = handle_arithmexpr($1, mul, $3);}
                |expr SLASH expr    {$$ = handle_arithmexpr($1, Div, $3);}
                |expr PERC expr     {$$ = handle_arithmexpr($1, mod, $3);}

relexpr:    expr GREATER expr   {$$ = handle_relexpr($1, if_greater, $3);}
            |expr GREQ expr     {$$ = handle_relexpr($1, if_greatereq, $3);}
            |expr LESS expr     {$$ = handle_relexpr($1, if_less, $3);}
            |expr LEQ expr      {$$ = handle_relexpr($1, if_lesseq, $3);}
            |expr EQ_EQ expr    {$$ = handle_relexpr($1, if_eq, $3);}
            |expr NEQ expr      {$$ = handle_relexpr($1, if_noteq, $3);}
            ;

boolexpr:   expr OR {$1 = expr_to_boolexpr($1);} M expr
            {
                printrule("boolexpr -> or");
                $5 = expr_to_boolexpr($5);

                $$ = newexpr(boolexpr_e);
                $$->sym = newtemp_reuse($1, $5, NULL);
                patchlist($1->falselist, $4);
                $$->truelist = mergelist($1->truelist, $5->truelist);
                $$->falselist = $5->falselist;
                // emit(or, $1, $3, $$);
            }
            |expr AND {$1 = expr_to_boolexpr($1);} M expr
            {
                printrule("boolexpr -> and");
                $5 = expr_to_boolexpr($5);

                $$ = newexpr(boolexpr_e);
                $$->sym = newtemp_reuse($1, $5, NULL);
                patchlist($1->truelist, $4);
                $$->truelist = $5->truelist;
                $$->falselist = mergelist($1->falselist, $5->falselist);
                // emit(and, $1, $3, $$);
            }
            ;

expr:   assignexpr  {printrule("expr -> assign");$$ = $1;}
        |term       {printrule("expr -> term");$$ = $1;}
        |arithmexpr {printrule("expr -> arithm");$$ = $1;}
        |relexpr    {printrule("expr -> relexpr");$$ = $1;}
        |boolexpr   {printrule("expr -> boolop");$$ = $1;}
        ;

term:   PARENTH_L expr PARENTH_R    {printrule("term -> (expr)");$$ = $2;}
        |MINUS expr %prec UMINUS
        {
            printrule("term -> uminus");
            check_arith($2, "uminus");
            $$ = newexpr(arithexpr_e);
            // $$->sym = istempexpr($2) ? $2->sym : newtemp();
            $$->sym = newtemp_reuse($2, NULL);
            emit(uminus, $2, NULL, $$);
        }
        |NOT expr
        {
            printrule("term -> !expr");
            $2 = expr_to_boolexpr($2);
            $$ = newexpr(boolexpr_e);
            // $$->sym = istempexpr($2) ? $2->sym : newtemp();
            $$->sym = newtemp_reuse($2, NULL);
            $$->truelist = $2->falselist;
            $$->falselist = $2->truelist;
            // emit(not, $2, NULL, $$);
        }
        |PLUS_PLUS lvalue   {printrule("term -> ++lv");$$ = handle_precrement($2, add);}
        |lvalue PLUS_PLUS   {printrule("term -> lv++");$$ = handle_postcrement($1, add);}
        |MINUS_MINUS lvalue {printrule("term -> --lv");$$ = handle_precrement($2, sub);}
        |lvalue MINUS_MINUS {printrule("term -> lv--");$$ = handle_postcrement($1, sub);}
        |primary            {printrule("term -> primary");$$ = $1;}
        ;

primary:    lvalue                          {printrule("primary -> lvalue");$$ = emit_iftableitem($1);}
            |call                           {printrule("primary -> call");$$ = $1;}
            |tablemake                      {printrule("primary -> tablemake");$$ = $1;}
            |PARENTH_L funcdef PARENTH_R
            {
                printrule("primary -> funcdef");
                $$ = newexpr(programfunc_e);
                $$->sym = $2;
            }
            |const                          {printrule("primary -> const");$$ = $1;}
            ;

lvalue: ID                  {printrule("lvalue -> ID");$$ = lvalue_expr(handle_lval($1));}
        |LOCAL ID           {printrule("lvalue -> local ID");$$ = lvalue_expr(handle_local_lval($2));}
        |DOUBLE_COLON ID    {printrule("lvalue -> global ID");$$ = lvalue_expr(handle_global_lval($2));}
        |member             {printrule("lvalue -> member");$$ = $1;}
        ;

member: lvalue DOT ID                       {printrule("member -> lvalue.id");$$ = member_item($1, $3);}
        |lvalue BRACKET_L expr BRACKET_R    {printrule("member -> lvalue[expr]");$$ = member_item_expr($1, $3);}
        |call DOT ID                        {printrule("member -> call.id");$$ = member_item($1, $3);}
        |call BRACKET_L expr BRACKET_R      {printrule("member -> call[expr]");$$ = member_item_expr($1, $3);}
        ;

call:   call PARENTH_L elist PARENTH_R
        {
            printrule("call -> call(elist)");
            $$ = make_call($1, $3);
        }
        |PARENTH_L funcdef PARENTH_R PARENTH_L elist PARENTH_R
        {
            printrule("call -> (funcdef)(elist)");
            $$ = make_call(lvalue_expr($2), $5);
        }
        |lvalue callsuffix
        {
            printrule("call -> lvalue callsuffix");
            $1 = emit_iftableitem($1);
            $2->elist = reverse_elist($2->elist);
            if ($2->method) {
                list_append($2->elist, $1);
                $1 = emit_iftableitem(member_item($1, $2->name));
            }
            $$ = make_call_r($1, $2->elist);
        }
        ;

callsuffix: normcall    {printrule("callsuffix -> normcall");$$ = $1;}
            |methodcall {printrule("callsuffix -> methodcall");$$ = $1;}
            ;

normcall:   PARENTH_L elist PARENTH_R
            {
                printrule("normcall");
                $$ = newcall($2, 0, "");
            };

methodcall: DOT_DOT ID PARENTH_L elist PARENTH_R
            {
                printrule("methodcall");
                $$ = newcall($4, 1, strdup($2));
            };

elist:  /* empty */         {printrule("elist -> empty");$$ = NULL;}
        |expr               {printrule("elist -> expr");$1 = short_circuit($1);$$ = $1;}
        |elist COMMA expr   {printrule("elist -> elist, expr");$3 = short_circuit($3);list_append($1, $3);}
        ;

tablemake:  BRACKET_L elist BRACKET_R       {printrule("tablemake");$$ = handle_tablemake_elist($2);}
            |BRACKET_L indexed BRACKET_R    {printrule("tablemake");$$ = handle_tablemake_idx($2);}
            ;

indexed:    indexedelem                 {printrule("indexed -> indexedelem");$$ = $1;}
            |indexed COMMA indexedelem  {printrule("indexed -> indexed, indexedelem");list_append($1, $3);}
            ;

indexedelem:    BRACE_L expr COLON expr BRACE_R {printrule("indexedelem -> {expr:expr}");$$ = $4;$$->index = $2;};

block:  BRACE_L {printrule("block"); scope++;} stmtlist BRACE_R {hide_scope(scope--);$$=$3;}
        |BRACE_L BRACE_R {printrule("block -> ε");$$ = make_stmt();}
        ;

funcname:   ID          {printrule("funcname -> id");$$ = $1;}
            | /*empty*/ {printrule("funcname -> ε");$$ = newhidfuncname();}
            ;

funcprefix:     FUNCTION funcname
                {
                    printrule("funcprefix");
                    if (check_funcdef($2)) {
                        $$ = new_func(strdup($2), USERFUNC_T, NULL);
                        $$->value.funcVal->iaddress = nextquadlabel();
                        emit_jump();
                        emit(funcstart, lvalue_expr($$), NULL, NULL);
                        push_stack(offsetStack, create_stack_node(currscopeoffset()));
                        // fprintf(stdout,"PUSHING OFFSET %d\n",currscopeoffset());
                        enter_scopespace();
                        resetformalargsoffset();
                        inFunction++;
                    }
                    else {
                        $$ = NULL;
                    }
                }
                ;

idlist: /*empty*/           {printrule("idlist -> empty");$$ = NULL;}
        |ID                 {printrule("idlist -> ID");$$ = handle_idlist_single($1);}
        |idlist COMMA ID    {printrule("idlist -> idlist, ID");$$ = handle_idlist_multiple($1, $3);}
        ;

funcargs:   PARENTH_L idlist PARENTH_R
            {
                printrule("funcargs");
                enter_scopespace();
                resetfunctionlocalsoffset();
                $$ = $2;
            }
            ;

funcblockstart: {push_loopcounter();};
funcblockend:   {pop_loopcounter();};

funcbody:   funcblockstart block funcblockend
            {
                printrule("funcbody");
                $$ = currscopeoffset();
                patchlist($2->returnlist, nextquadlabel());
                exit_scopespace();
            }
            ;

funcdef:    funcprefix funcargs funcbody
            {
                if ($1 != NULL) {
                    printrule("funcdef");
                    inFunction--;
                    exit_scopespace();

                    $1->value.funcVal->total_locals = $3;
                    unsigned prev_offset = pop_stack(offsetStack);
                    // fprintf(stdout,"GOT OFFSET %d\n",prev_offset);
                    $1->value.funcVal->arguments = $2;

                    // restore_curr_scope_offset(prev_offset);
                    emit(funcend, lvalue_expr($1), NULL, NULL);

                    patchlabel($1->value.funcVal->iaddress, nextquadlabel());
                }
                $$ = $1;
            }
            ;


number: REAL {printrule("real number");$$ = newexpr_constreal($1);}
        |INT {printrule("int number");$$ = newexpr_constint($1);}
        ;

const:  STRING      {$$ = newexpr_conststring($1);}
        | number    {$$ = $1;}
        | TRUE      {$$ = newexpr_constbool(1);}
        | FALSE     {$$ = newexpr_constbool(0);}
        | NIL       {$$ = newexpr(nil_e);}
        ;

ifprefix:   IF PARENTH_L expr PARENTH_R
            {
                printrule("ifprefix");
                $3 = short_circuit($3);
                emit_l(if_eq, newexpr_constbool(1), $3, NULL, nextquadlabel()+2);
                $$ = nextquadlabel();
                emit_jump();
            }
            ;

elseprefix: ELSE
            {
                printrule("elseprefix");
                $$ = nextquadlabel();
                emit_jump();
            }
            ;

ifstmt: ifprefix stmt {
            printrule("ifstmt -> if");
            patchlabel($1, nextquadlabel());
            $$ = $2;
        }
        |ifprefix stmt elseprefix stmt  {
            printrule("ifstmt -> ifelse");
            patchlabel($1, $3 + 1);
            patchlabel($3, nextquadlabel());

            $$ = make_stmt();
            $$->breaklist = mergelist($2->breaklist, $4->breaklist);
            $$->contlist = mergelist($2->contlist, $4->contlist);
            $$->returnlist = mergelist($2->returnlist, $4->returnlist);
        }
        ;

loopstart:  {printrule("loopstart");++loopcounter;};
loopend:    {printrule("loopend");--loopcounter;};
loopstmt:   loopstart stmt loopend  {$$ = $2;};

whilestart: WHILE   {$$ = nextquadlabel();};

whilecond:  PARENTH_L expr PARENTH_R
            {
                printrule("whilecond");
                $2 = short_circuit($2);
                emit_l(if_eq, $2, newexpr_constbool(1), NULL, nextquadlabel()+2);
                $$ = nextquadlabel();
                emit_jump();
            }
            ;

whilestmt:  whilestart whilecond loopstmt
            {
                printrule("whilestmt");
                emit_l(jump, NULL, NULL, NULL, $1);
                patchlabel($2, nextquadlabel());

                patchlist($3->breaklist, nextquadlabel());
                patchlist($3->contlist, $1);
                $$ = $3;
            }
            ;

N:  {printrule("N");$$ = nextquadlabel();emit_jump();};

M:  {printrule("M");$$ = nextquadlabel();};

forprefix:  FOR PARENTH_L elist SEMICOLON M expr SEMICOLON
            {
                printrule("forprefix");
                $$ = malloc(sizeof(struct forloopStruct));
                $$->test = $5;
                $6 = short_circuit($6);
                $$->enter = nextquadlabel();
                emit_l(if_eq, $6, newexpr_constbool(1), NULL, 0);
            }
            ;

forstmt:    forprefix N elist PARENTH_R N loopstmt N
            {
                printrule("forstmt");
                patchlabel($1->enter, $5+1);
                patchlabel($2, nextquadlabel());
                patchlabel($5, $1->test);
                patchlabel($7, $2+1);

                patchlist($6->breaklist, nextquadlabel());
                patchlist($6->contlist, $2+1);
                $$ = $6;
            }
            ;


%%


int main(int argc, char **argv) {
    if (!(alpha_yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot open file %s for reading. Exiting.\n", argv[1]);
        return 1;
    }

    if (!(alpha_yyout = fopen("yyout.txt", "w"))) {
    // if (!(alpha_yyout = stdout)) {
        fprintf(stderr, "Cannot open file for writing yacc outputs. ");
        fprintf(stderr, "Reverting to stdout instead.\n");
        alpha_yyout = stdout;
    }


    // if (argc > 2) {
    //     if (!(quadout = fopen(argv[2], "w"))) {
    //         fprintf(stderr, "Cannot open file %s for writing quads. ", argv[2]);
    //         fprintf(stderr, "Reverting to stdout instead.\n");
    //         quadout = stdout;
    //     }
    // } else {
    //     // quadout = stdout;
    // }

    push_loopcounter();

    for (int i = 0; i < 12; i++)
        handle_funcdef_lib(lib_funcs[i]);

    alpha_yyparse();
    fprint_scope_lists(alpha_yyout);
    fprint_symbol_table(alpha_yyout);
    fclose(alpha_yyout);

    quadout = fopen("quads.txt", "w");
    print_quads(quadout);
    fclose(quadout);

    // revert stdout to file named "debugout.txt"
    FILE* debugout = fopen("debugout.txt", "w");
    // FILE* stdout_b = stdout;
    out = stdout;
    stdout = debugout;
    // out = fopen("out.txt", "w");

    generate_target_code();

    instr_out = fopen("tcode.txt", "w");
    print_instructions(instr_out);
    fclose(instr_out);

    avm_initialize();
	while(!executionFinished){
        fprintf(stdout,"EXECUTING INSTRUCTION\n");
		execute_cycle();
	}


    // createbin();
    fclose(out);

    return 0;
}

void alpha_yyerror(const char *msg) {
    fprintf(stderr, "\e[48;2;200;0;0m""error: %s at line %d""\x1b[0m""\n", msg, yyline);
    // exit(1);
}

int yywrap() {
    return 1;
}