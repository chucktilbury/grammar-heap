# This is the grammar for the input to PGEN
# written using the syntax that PGEN accepts.

grammar:
    ((rule | directive)+){} # code runs one time after match completes
    ;

directive:
    ((PRETEXT CODE_BLOCK){}) |
    ((POSTTEXT CODE_BLOCK){}) |
    ((PRECODE CODE_BLOCK){}) |
    ((POSTCODE CODE_BLOCK){}) |
    ((TERM_DEF CODE_BLOCK){}) |
    ((NTERM_DEF CODE_BLOCK){}) |
    ((PROVIDES CODE_BLOCK){}) |
    ((REQUIRES CODE_BLOCK){})
    ;

rule:
    (NON_TERMINAL COLON expression SEMICOLON){}
    ;

expression_primary:
    ((NON_TERMINAL){}) |
    ((TERMINAL_SYMBOL){}) |
    ((TERMINAL_KEYWORD){}) |
    ((TERMINAL_OPER){}) |
    ((OPAREN expression CPAREN){}) |
    ((OPAREN expression CPAREN CODE_BLOCK){})
    ;

expression:
    (function BAR function){}
    ;

function:
    ((expression_primary+ STAR){}) |
    ((expression_primary+ PLUS){}) |
    ((expression_primary+ QUESTION){})
    ;


