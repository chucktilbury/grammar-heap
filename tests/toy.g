# This is a grammar for the Toy programming language, which is a subset of
# the Simple programming language.

program (
    program_item+
)

program_item (
    data_def |
    data_decl |
    function_def |
    import_statement |
    start_statement
)

type_name (
    ('int' | 'integer') |
    'float' |
    ('bool' | 'boolean') |
    ('str' | 'string')
)

data_decl (
    type_name IDENTIFIER
)

data_def (
    type_name IDENTIFIER '=' expression
)

function_def_name (
    ('nothing' | type_name) IDENTIFIER
)

function_def (
    function_def_name '(' ( data_decl ( ',' data_decl )* )? ')' func_body
)

import_statement (
    'import' LITERAL_STRING
)

start_statement (
    'start' func_body
)

formatted_string (
    LITERAL_STRING ( '(' ( expression ( ',' expression )* )? ')' )?
)

expression (
    and_expr ( ('|' | 'or' ) and_expr )*
)

and_expr (
    equ_expr ( ('&' | 'and' ) equ_expr )*
)

equ_expr (
    ( magnitude_expr ( ( '==' | 'equ' ) magnitude_expr )* ) |
    ( magnitude_expr ( ( '!=' | 'nequ' ) magnitude_expr )* )
)

magnitude_expr (
    ( sum_expr ( ( '<' | 'lt' ) sum_expr )* ) |
    ( sum_expr ( ( '>' | 'gt' ) sum_expr )* ) |
    ( sum_expr ( ( '<=' | 'lte' ) sum_expr )* ) |
    ( sum_expr ( ( '>=' | 'gte' ) sum_expr )* )
)

sum_expr (
    ( product_expr ( '+' product_expr )* ) |
    ( product_expr ( '-' product_expr )* )
)

product_expr (
    ( pow_expr ( '*' pow_expr )* ) |
    ( pow_expr ( '/' pow_expr )* ) |
    ( pow_expr ( '%' pow_expr )* )
)

pow_expr (
    unary_minus_expr ( '^' unary_minus_expr )*
)

unary_minus_expr (
    ( '-' unary_not_expr )*
)

unary_not_expr (
    ( ( '!' | 'not' ) primary_expr )*
)

primary_expr (
    IDENTIFIER |
    LITERAL_INT |
    LITERAL_FLOAT |
    LITERAL_BOOL |
    formatted_string |
    '(' expression ')'
)

func_body (
    '{' ( func_body_item | func_body )+ '}'
)


func_ref (
    IDENTIFIER '(' ( expression ( ',' expression )* )? ')'
)

assignment (
    IDENTIFIER '=' expression
)

loop_body_item (
    func_body_item |
    'continue' |
    'break'
)

loop_body (
    '{' ( loop_body_item | loop_body )* '}'
)

while_statement (
    'while' ( '(' expression? ')' )? loop_body
)

do_statement (
    'do' loop_body 'while' ( '(' expression? ')' )?
)

if_statement (
    if_clause ( else_clause )* ( final_else )?
)

if_clause (
    'if' '(' expression ')' func_body
)

else_clause (
    'else' '(' expression ')' func_body
)

final_else (
    'else' ( '(' expression? ')' )? func_body
)

inline_statement (
    'inline' '{' LITERAL_TEXT '}'
)

return_statement (
    ('return' ( '(' expression? {} ')' )?{}) {// this is { some} code, too}
)

%pretext {
    // this is some pretext code
}

%precode {
typedef struct {
    int foo bar;
    int baz_haza;
} blart:
}

func_body_item (
    data_decl |
    data_def |
    func_ref |
    assignment |
    while_statement |
    do_statement |
    if_statement |
    inline_statement { // is this acceptable? } |
    return_statement
)

