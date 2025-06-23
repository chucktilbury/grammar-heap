# Simple calculator with code

# This defines the data type that the non-terminals will have, similar to the
# %union for YACC style grammars. The code in the grammar is responsible for
# calling out the intended type.
%nterm_def {
    int value;
}

# This is like the %nterm but for terminal symbols. This will normally be some
# kind of token data structure that has information like the name and the line
# number.
%term_def {
    int number;
}

# The $$ symbol refers to the value of the non-terminal that is being matched.
# The elements in the rule are matched using $1, $2, etc. Similar to YACC.
expr
    : sum { printf("%d\n", $1.value); }
    ;

# In this production $1 is a non-terminal, $2 is a terminal, and $3 is a non-
# terminal.
sum
    : product '+' product { $$ = $1.value + $3.value; }
    | product '-' product { $$ = $1.value - $3.value; }
    ;

product
    : value '*' value { $$ = $1.value * $3.value; }
    | value '/' value { $$ = $1.value / $3.value; }
    ;

# The non-terminal value is addigned to a terminal
value
    : NUMBER { $$ = $1.number; }
    | '(' sum ')' { $$ = $2.value}
    ;

