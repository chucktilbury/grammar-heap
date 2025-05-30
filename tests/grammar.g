
# This is a grammar for the input to pgen.

grammar (
    rule+
)

rule (
    NON_TERMINAL grouping_function+
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

or_function (
    rule_element PIPE rule_element
)

one_or_more_function (
    rule_element PLUS
)

zero_or_more_function (
    rule_element STAR
)

zero_or_one_function (
    rule_element QUESTION
)

grouping_function (
    OPAREN rule_element+ CPAREN
)

