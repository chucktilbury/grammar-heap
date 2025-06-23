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

"%pretext"  { return PRETEXT; }
"%posttext" { return POSTTEXT; }
"%precode"  { return PRECODE; }
"%postcode" { return POSTCODE; }
"%term_def" { return TERM_DEF; }
"%nterm_def" { return NTERM_DEF; }
"%provides" { return PROVIDES; }
"%requires" { return REQUIRES; }

"+" { return PLUS; }
"*" { return STAR; }
"?" { return QUESTION; }
"|" { return PIPE; }
"(" { return OPAREN; }
")" { return CPAREN; }

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
        yylval.token = create_token(raw_string(strbuf), CODE_BLOCK);
        return CODE_BLOCK;
    }
}

[a-z_][a-z0-9_]* {
    yylval.token = create_token(yytext, NON_TERMINAL);
    return NON_TERMINAL;
}

[A-Z_][A-Z0-9_]* {
    yylval.token = create_token(yytext, TERMINAL_SYMBOL);
    return TERMINAL_SYMBOL; \
}

\'[a-zA-Z_][a-zA-Z0-9_]*\' {
    yylval.token = create_token(yytext, TERMINAL_KEYWORD);
    return TERMINAL_KEYWORD;
}

\'[^a-zA-Z0-9_\']+\' {
    yylval.token = create_token(yytext, TERMINAL_OPER);
    return TERMINAL_OPER;
}

#[^\n]* { /* ignore comment */ }

[ \t\r\n\v\f]* { /* ignore whitespace */ }

%%

