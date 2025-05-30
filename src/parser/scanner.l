%{

#include "tokens.h"
#include "parser.h"

%}

%option yylineno
%option noinput
%option nounput
%option noyywrap
    // for cmake...
    //%option header-file="scanner.h"
    //%option outfile="scanner.c"

%%

"+" { return PLUS; }
"*" { return STAR; }
"?" { return QUESTION; }
"|" { return PIPE; }
"(" { return OPAREN; }
")" { return CPAREN; }

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

