# This is a grammar for the input to gh.

grammar (
    (rule | directive)+
)

rule (
    NON_TERMINAL grouping_function+
)

# This is intended to contain things like data structures that exist
# outside of a function. The curly braces are not copied to the output.
pre_text (
    '%pretext' code_block
)

# This defines the internals of a function that is executed before the
# parser begins execution. Use this to initialize a scanner or whatnot.
pre_code (
    '%precode' code_block
)

# This code is executed in a function after the parser has run.
post_code (
    '%postcode' code_block
)

directive (
    pre_text |
    pre_code |
    post_code
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
    grouping_function |
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
    OPAREN rule_element+ CPAREN code_block?
)

# Greedy expression consumes all of the text up untill the {} match.
# Copied directly into the output after any group. No syntax checking
# of any kind is done on a code block, other than matching the {}'s.
code_block (
    OCBRACE ANY_TEXT CCBRACE
)
