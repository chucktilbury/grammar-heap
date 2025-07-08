# a completely recursive grammar
start:
    (expr+) {}
    ;

# the order of the elements establishes the precedence.
expr:
    (sum) {} |
    (factor) {} |
    (primary) {}
    ;

sum:
    (expr '+' expr) {} |
    (expr '-' expr) {}
    ;

factor:
    (expr '*' expr) {} |
    (expr '/' expr) {}
    ;

primary:
    NUMBER |
    ('(' expr ')')
    ;
