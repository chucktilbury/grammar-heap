
# This is a grammar to demonstrate and test the input to pgen.

# Directives can be placed anywhere in the grammar, but only one instance
# of each one is allowed.

<<<<<<< HEAD
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
=======
# This is intended to contain things like data structures that exist
# outside of a function that need to be at the start of the parser
# definition. The curly braces are not copied to the output.
pre_text (
    PRETEXT code_block
)

# This defines test outside a function that goes at the end of the
# generated parser file.
post_text (
    POSTTEXT code_block
)

# This defines the internals of a function that is executed before the
# parser begins execution. Use this to initialize a scanner or whatnot.
pre_code (
    PRECODE code_block
)

# This code is executed in a function after the parser has run. Used to
# free memory or close files.
post_code (
    POSTCODE code_block
)

# This defines the data structure for terminal symbols.
term_def (
    TERM_DEF code_block
)

# This defines the data structure for non-terminal symbols.
nterm_def (
    NTERM_DEF code_block
)

# This is definitions that will be copied to the generated header file.
provides (
    PROVIDES code_block
)

# This is similar to the pre_text production but more clearly specifies
# definitions for the generated parser that are created by the user code.
requires (
    REQUIRES code_block
)

directive (
    pre_text |
    post_text |
    pre_code |
    post_code |
    requires |
    provides |
    term_def |
    nterm_def
)

rule_element (
    NON_TERMINAL |
    TERMINAL_SYMBOL |
    TERMINAL_KEYWORD |
    TERMINAL_OPER |
    or_function |
    one_or_more_function |
    zero_or_more_function |
    zero_or_one_function |
    grouping_function
)
>>>>>>> before_grammar_switch

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
