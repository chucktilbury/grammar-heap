# a completely recursive grammar
start:
    (expr+) {code}
    ;

# the order of the elements establishes the precedence.
expr:
    (sum) {code} |
    (factor) {code} |
    (primary) {code}
    ;

sum:
    (expr '+' expr) {code} |
    (expr '-' expr) {code}
    ;

factor:
    (expr '*' expr) {code} |
    (expr '/' expr) {code}
    ;

primary:
    NUMBER |
    ('(' expr ')')
    ;
