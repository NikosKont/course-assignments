%{
    #include "parser.h"
    #include <assert.h>

    unsigned int comm_level = 0;
    unsigned int start;
    unsigned int nested[509];
    char strbuf[509];
    char *strbuf_p;

    int keyword_subcat();
    int op_subcat();
    int punc_subcat();
    int str_end();
    void block_end();
    void error();
    void error_l();
%}

%option outfile="scanner.c"
%option prefix="alpha_yy"
%option noyywrap
%option yylineno

keyword             "if"|"else"|"while"|"for"|"function"|"return"|"break"|"continue"|"and"|"not"|"or"|"local"|"true"|"false"|"nil"
comm_single         "//"(.*)"\n"
operator            "=="|"="|"++"|"+"|"--"|"-"|"*"|"/"|"%"|"!="|">="|">"|"<="|"<"
integer             [0-9]+
real                [0-9]*\.[0-9](e|E)?-?[0-9]*
punctuation         "{"|"}"|"["|"]"|"("|")"|";"|","|":"|"::"|"."|".."
identifier          [a-zA-Z][a-zA-Z_0-9]*

%x comm
%x str


%%


{keyword}           {alpha_yylval.strVal = strdup(alpha_yytext);return keyword_subcat(alpha_yytext);}
{operator}          {alpha_yylval.strVal = strdup(alpha_yytext);return op_subcat(alpha_yytext);}
{punctuation}       {alpha_yylval.strVal = strdup(alpha_yytext);return punc_subcat(alpha_yytext);}
{integer}           {alpha_yylval.intVal = atoi(yytext); return INT;}
{real}              {alpha_yylval.realVal = atof(yytext); return REAL;}
{identifier}        {alpha_yylval.strVal = strdup(alpha_yytext); return ID;}
{comm_single}       {/* ignore */}


\"                  {BEGIN(str);strbuf_p = strbuf;start = alpha_yylineno;}
<str>{
    \"                      {return str_end();}
    <<EOF>>                 error_l("Unterminated string", start);
    \\a                     *strbuf_p++ = '\a';
    \\b                     *strbuf_p++ = '\b';
    \\e                     *strbuf_p++ = '\e';
    \\f                     *strbuf_p++ = '\f';
    \\n                     *strbuf_p++ = '\n';
    \\\n                    *strbuf_p++ = '\n';
    \n                      *strbuf_p++ = '\n';
    \\r                     *strbuf_p++ = '\r';
    \\t                     *strbuf_p++ = '\t';
    \\v                     *strbuf_p++ = '\v';
    \\\\                    *strbuf_p++ = '\\';
    \\'                     *strbuf_p++ = '\'';
    \\\"                    *strbuf_p++ = '\"';
    \\?                     *strbuf_p++ = '\?';
    \\.                     {
                                *strbuf_p++ = '\\';
                                *strbuf_p++ = alpha_yytext[1];
                                fprintf(stderr, "warning: Unrecognized escape sequence \"%s\" at line %d\n", alpha_yytext, alpha_yylineno);
                            }

    [^\\\n\"]+              { // legal string content
                                char *temp = alpha_yytext;
                                while ( *temp )
                                    *strbuf_p++ = *temp++;
                            }
}


"/*"                {BEGIN(comm);start = alpha_yylineno;} /* */
<comm>{
    "/*"                    {++comm_level;nested[comm_level] = alpha_yylineno;}
    "*"+"/"                 {block_end(alpha_yylval);}
    "*"+|[^/*\n]+|[/]|\n    ; /* */
    <<EOF>>                 error_l("Unterminated comment", start);
}

[" "\t\n\r\f\v]     ; /* skip whitespace */
.                   {
                        sprintf(strbuf, "Undefined input: \"%s\"", alpha_yytext);
                        error(strbuf);
                    }
<<EOF>>             return EOF;


%%

int search_array(char* s, char** arr, int size) {
    int i;
    for (i = 0; i < size; i++)
        if (strcmp(s, arr[i]) == 0) return i;
    return -1;
}

char* keyword_raw[] = {"if",     "else",  "while",    "for",   "function",
                       "return", "break", "continue", "and",   "not",
                       "or",     "local", "true",     "false", "nil"};

enum yytokentype keywords[] = {IF,     ELSE,  WHILE, FOR,   FUNCTION,
                               RETURN, BREAK, CONT,  AND,   NOT,
                               OR,     LOCAL, TRUE,  FALSE, NIL};

int keyword_subcat(char* s) {
    int idx = search_array(s, keyword_raw, 15);
    assert(idx != -1);
    return keywords[idx];
}

char* op_raw[] = {
    "=", "+", "-", "*", "/", "%", "==", "!=", "++", "--", ">", "<", ">=", "<="};

enum yytokentype operators[] = {EQUAL,   PLUS,  MINUS, STAR,      SLASH,
                                PERC,    EQ_EQ, NEQ,   PLUS_PLUS, MINUS_MINUS,
                                GREATER, LESS,  GREQ,  LEQ};

int op_subcat(char* s) {
    int idx = search_array(s, op_raw, 14);
    assert(idx != -1);
    return operators[idx];
}

char* punc_raw[] = {"{", "}", "[", "]",  "(", ")",
                    ";", ",", ":", "::", ".", ".."};

enum yytokentype puncutation[] = {BRACE_L,   BRACE_R,      BRACKET_L, BRACKET_R,
                                  PARENTH_L, PARENTH_R,    SEMICOLON, COMMA,
                                  COLON,     DOUBLE_COLON, DOT,       DOT_DOT};

int punc_subcat(char* s) {
    int idx = search_array(s, punc_raw, 12);
    assert(idx != -1);
    return puncutation[idx];
}

int str_end() {
    *strbuf_p = '\0';
    BEGIN(INITIAL);
    alpha_yylval.strVal = strdup(strbuf);
    return STRING;
}

void block_end() {
    if (comm_level) {
        comm_level--;
    } else {
        BEGIN(INITIAL);
    }
}

void error_l(char* msg, unsigned int line) {
    fprintf(stderr, "error: %s at line %d\n", msg, line);
    exit(1);
}
void error(char* msg) { error_l(msg, yylineno); }