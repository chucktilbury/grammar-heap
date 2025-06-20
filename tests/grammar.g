
# This is a grammar to demonstrate and test the input to pgen.

# Directives can be placed anywhere in the grammar, but only one instance
# of each one is allowed.

# This code is placed at the beginning of the generated header. It
# should be used to declare ant functions or data structures that are
# user-created.
%header {}

# This code is placed at the beginning of the generated parser
# and outside of any function. Any custom data structures that are
# used in the user-defined code should be declared here.
%pretext {}

# This code is actually run before the parser starts. The user can open
# files or do other initialization in this code or in the code that calls
# the parser.
%precode {}

# This code is actually run after the parser ends and can be used to
# do any cleanup required at that time.
%postcode {
    // This is some silly text to simulate a bit of C code.
    // This text is passed exactly as defined to the output, but without
    // the opening and closing curly braces.
    while(1) {
        if(0 == (blart=the_fart)) {
            while(1+2/3+zoob) {
                kill_the_termites();
            }
        }
    }
}

# Grammar rules have the same general format as a YACC grammar except that
# there are some added directives

# The %list() directive specifies one or more of whatever is inside of the
# parentheses. The purpose of this directive is to make it easier to emit
# lists without the issues of left or right recursion. Instead of recursion
# the list is generated in a loop. Note that left recusion is not allowed in
# things like expressions due to the infinite recusion problem.

# The %error{} directive forces a syntax error if the rule cannt be matched
# and executes the code that was defined for it.

grammar
    : rule {
        }
    | grammar rule {
        }
    ;

directive
    : PRETEXT CODE_BLOCK {
        }
    | PRECODE CODE_BLOCK {
        }
    | POSTCODE CODE_BLOCK {
        }
    | HEADERCODE CODE_BLOCK {
        }
    ;

rule
    : NON_TERMINAL ':' alternative_list ';' {
        }
    | directive {
        }
    ;

alternative
    : rule_element_list {
        }
    | rule_element_list CODE_BLOCK {
        }
    | %error {
        }
    ;

alternative_list
    : alternative {
        }
    | alternative %list('|' alternative) {
        }
    ;

rule_element_list
    : rule_element {
        }
    | rule_element %list(rule_element) {
        }
    ;

rule_element
    : TERMINAL_SYMBOL {
        }
    | TERMINAL_KEYWORD {
        }
    | TERMINAL_OPER {
        }
    | NON_TERMINAL {
        }
    | list_clause {
        }
    ;

list_clause
    : LIST '(' rule_element_list ')' {}
    ;
