# This is a grammar for the input to pgen.

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

    ;

rule
    : NON_TERMINAL ':' alternative_list ';' {
        }
    | NON_TERMINAL ':' alternative_list '|' ERROR CODE_BLOCK ';' {
        }
    | directive {
        }
    ;

alternative
    : rule_element_list {
        }
    | rule_element_list CODE_BLOCK {
        }
    ;

alternative_list
    : alternative {
        }
    | alternative_list '|' alternative {
        }
    ;

rule_element_list
    : rule_element {
        }
    | rule_element_list rule_element {
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
    ;
