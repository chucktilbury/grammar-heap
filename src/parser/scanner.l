%{

#include "ast.h"
#include "tokens.h"
#include "parser.h"
#include "string_buffer.h"

int depth;
string_t* strbuf = NULL;

%}

%option yylineno
%option noinput
%option nounput
%option noyywrap
    // for cmake...
    //%option header-file="scanner.h"
    //%option outfile="scanner.c"

%x CODEBLOCK

%%

"%pretext"  {
    //yylval.terminal = create_token(yytext, PRETEXT);
    yylval.token = create_token(yytext, TERM_PRETEXT);
    return PRETEXT;
}

"%precode"  {
    //yylval.terminal = create_token(yytext, PRECODE);
    yylval.token = create_token(yytext, TERM_PRECODE);
    return PRECODE;
}

"%postcode"  {
    //yylval.terminal = create_token(yytext, POSTCODE);
    yylval.token = create_token(yytext, TERM_POSTCODE);
    return POSTCODE;
}

"%header" {
    //yylval.terminal = create_token(yytext, HEADER);
    yylval.token = create_token(yytext, TERM_HEADER);
    return HEADERCODE;
}

"%error" {
    //yylval.terminal = create_token(yytext, ERROR);
    yylval.token = create_token(yytext, TERM_ERROR);
    return ERROR;
}

"%list" {
    //yylval.terminal = create_token(yytext, LIST);
    yylval.token = create_token(yytext, TERM_LIST);
    return LIST;
}

";" { return ';'; }
"|" { return '|'; }
":" { return ':'; }
"(" { return '('; }
")" { return ')'; }

"{" {
    depth = 0;
    if(strbuf == NULL)
        strbuf = create_string(NULL);
    else
        clear_string(strbuf);
    BEGIN(CODEBLOCK);
}

<CODEBLOCK>"{" {
    depth++;
    append_string_char(strbuf, '{');
}

<CODEBLOCK>[^{}\n] {
    append_string(strbuf, yytext);
}

<CODEBLOCK>\n {
    // so line numbers continue to work
    append_string_char(strbuf, '\n');
}

<CODEBLOCK>"}" {
    if(depth > 0) {
        depth--;
        append_string_char(strbuf, '}');
    }
    else {
        BEGIN(INITIAL);
        //yylval.terminal = create_token(raw_string(strbuf), CODE_BLOCK);
        yylval.token = create_token(raw_string(strbuf), TERM_CODE_BLOCK);
        return CODE_BLOCK;
    }
}

[a-z_][a-z0-9_]* {
    //yylval.terminal = create_token(yytext, NON_TERMINAL);
    yylval.token = create_token(yytext, TERM_NON_TERMINAL);
    return NON_TERMINAL;
}

[A-Z_][A-Z0-9_]* {
    //yylval.terminal = create_token(yytext, TERMINAL_SYMBOL);
    yylval.token = create_token(yytext, TERM_TERMINAL_SYMBOL);
    return TERMINAL_SYMBOL;
}

\'[a-zA-Z_][a-zA-Z0-9_]*\' {
    //yylval.terminal = create_token(yytext, TERMINAL_KEYWORD);
    yylval.token = create_token(yytext, TERM_TERMINAL_KEYWORD);
    return TERMINAL_KEYWORD;
}

\'[^a-zA-Z0-9_\']+\' {
    //yylval.terminal = create_token(yytext, TERMINAL_OPER);
    yylval.token = create_token(yytext, TERM_TERMINAL_OPER);
    return TERMINAL_OPER;
}

#[^\n]* { /* ignore comment */ }

[ \t\r\n\v\f]* { /* ignore whitespace */ }

%%

