%{

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
    set_token(create_token(yytext, PRETEXT));
    return PRETEXT;
}

"%posttext" {
    set_token(create_token(yytext, POSTTEXT));
    return POSTTEXT;
}

"%precode"  {
    set_token(create_token(yytext, PRECODE));
    return PRECODE;
}

"%postcode" {
    set_token(create_token(yytext, POSTCODE));
    return POSTCODE;
}

"%term_def" {
    set_token(create_token(yytext, TERM_DEF));
    return TERM_DEF;
}

"%nterm_def" {
    set_token(create_token(yytext, NTERM_DEF));
    return NTERM_DEF;
}

"%provides" {
    set_token(create_token(yytext, PROVIDES));
    return PROVIDES;
}

"%requires" {
    set_token(create_token(yytext, REQUIRES));
    return REQUIRES;
}


"+" {
    set_token(create_token(yytext, PLUS));
    return PLUS;
}

"*" {
    set_token(create_token(yytext, STAR));
    return STAR;
}

"?" {
    set_token(create_token(yytext, QUESTION));
    return QUESTION;
}

"|" {
    set_token(create_token(yytext, PIPE));
    return PIPE;
}

"(" {
    set_token(create_token(yytext, OPAREN));
    return OPAREN;
}

")" {
    set_token(create_token(yytext, CPAREN));
    return CPAREN;
}

":" {
    set_token(create_token(yytext, COLON));
    return COLON;
}

";" {
    set_token(create_token(yytext, SEMICOLON));
    return SEMICOLON;
}


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
        set_token(create_token(raw_string(strbuf), CODE_BLOCK));
        return CODE_BLOCK;
    }
}

[a-z_][a-z0-9_]* {
    set_token(create_token(yytext, NON_TERMINAL));
    return NON_TERMINAL;
}

[A-Z_][A-Z0-9_]* {
    set_token(create_token(yytext, TERMINAL_SYMBOL));
    return TERMINAL_SYMBOL; \
}

\'[a-zA-Z_][a-zA-Z0-9_]*\' {
    set_token(create_token(yytext, TERMINAL_KEYWORD));
    return TERMINAL_KEYWORD;
}

\'[^a-zA-Z0-9_\']+\' {
    set_token(create_token(yytext, TERMINAL_OPER));
    return TERMINAL_OPER;
}

#[^\n]* { /* ignore comment */ }

[ \t\r\n\v\f]* { /* ignore whitespace */ }

. {
    fprintf(stderr, "scanner error: %d: unknown character encountered: %c (0x%02X)", yylineno, yytext[0], yytext[0]);
    exit(1);
}

%%
