# heap
Top-down parser generator that uses a FSM instead of LALR tables or recursive decent.

This uses Bison and Flex to parse a grammar which is composed of simple regular expressions. The parser that is generated uses a single finite state machine to pars the input based upon the grammar. Since it is not recursive, it has no problem with left recursion and any grammer that you can represent with this syntax should be parsable. 

The input is arranged in very simple regulae expressions that are similar to the commonly used format. 

* A grammar consists of a list of rules.
* A rule is a non-terminal followed by an expression that is enclosed in paraentheses.
* An expression can have any combination of functions, terminals, and non-terminals.
* Whitespace is completely ignored except where it separates symbols.
* Comments start with a ``#`` character and end at a newline.

This is a parser generator that generates a FSM parser. It is a work in progress. The goal it to create a working parser and AST generator/traverse from a simplified grammar. A scanner is not generated (yet) and there are specific rules to relate to the case and placement of symbols in the grammar.

* A ``[a-z_][a-z0-9_]*`` is a non terminal symbol. It is defined exactly once and then may be referenced any number of times.
* A ``[A-Z_][A-Z0-9_]*`` is a terminal that must be constructed in the scanner. So when the parser generator encounters a token that looks like ``WOMBAT99`` it will create a token type of ``TOK_WOMBAT99`` and assign a number to it and the generated parser will expect to see a ``TOK_WOMBAT99`` in the input stream. The scanner for the generated parser will return that as a token type when it scans whatever a ``TOK_WOMBAT99`` is supposed to represent.
* A ``\'[a-zA-Z_][a-zA-Z0-9_]*\'`` is a keyword definition such as ``'while'`` or ``'import'``. A token type is created such as ``TOK_WHILE`` and assigned a number. The generated parser refers to it by that symbolic designation.
* A ``\'[^a-zA-Z0-9_\']+\'`` is an operator. For example the string ``'!='`` translates to ``TOK_BANG_EQUAL`` in the generated parser. The scanner for the generated parser must recognize that string and return the token type.
* Comments start with a ``#`` character and continue to the end of the line.
* All non-printable characters are ignored.
* See ``pgen/src/parser/scanner.l`` for the complete scanner rules.

The generated parser simply generates the AST and the code to traverse it. The person using it will use the traverse functions to implement whatever they are trying to do with the parser.

## Requirements

* flex and bison
* cmake
* doxygen
* graphviz (for doxygen)
* Any ANSI C compiler. I am currently using clang 18.
* git (of course)

Here is an example grammer:
```
# This is a grammar for the input to heap.

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


```
