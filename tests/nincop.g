# This is a new version of TOY that is intended to be useful as a not-oop
# application development language.

program (
    program_item* |
    start_block
)

program_item (
    import_statement |
    data_declaration |
    data_decl_assignment |
    func_definition |
    type_definition |
    namespace_definition |
    scope_operator
)

# Entry point of the program. Exactly one must exist in the entire namespace.
start_block (
    'start' func_body
)

# Format sections in the text look like {IDENTIFIER} and are automatically
# converted and replaced.
string_param (
    IDENTIFIER '=' expression
)

# Strings are formatted as part of the language.
formatted_string (
    STRING_LITERAL ('(' (string_param (',' string_param)*)? ')')?
)

# Names of definitions (not references)
compound_identifier (
    IDENTIFIER ('.' IDENTIFIER)*
)

# In a program module this controls whether import will add it to the
# searchable namespace. In the context of a struct, functions that are
# defined in the struct have access to private attributes and can call
# private functions, but they cannot be accessed otherwise. Default is
# private.
scope_operator (
    'protected' |
    'public' |
    'private'
)

# Comprehensive list of native types and user-defined types checked at
# compile time.
type_name (
    ('int' | 'integer') |
    ('bool' | 'boolean') |
    ('str' | 'string') |
    'float' |
    'list' |
    'hash' |
    compound_identifier
)

# Open another module and bring it into this namespace.
# If the 'as' clause is present then create a namespace for the import.
import_statement (
    'import' STRING_LITERAL ('as' IDENTIFIER)?
)

data_name_decl (
    type_name IDENTIFIER
)

# Declare a data element without initializing it.
data_declaration (
    'const'? data_name_decl
)

# Declare a data element with a compile-time constant to assign to it.
data_decl_assignment (
    data_declaration '=' expression
)

func_parameters (
    '(' ( data_name_decl (',' data_name_decl)* )? ')'
)

func_identifier (
    IDENTIFIER |
    operator_type
)

func_type (
    type_name |
    'nothing'
)

# Forward declaration or definition of a function to get it into the namespace.
# Function overloading is supported. The compound identifier is a path to the
# type for which the function is defined. It is is absent, then the function
# is defined in the root scope.
func_definition (
    func_type compound_identifier? func_identifier func_parameters func_body?
)

# These are used when overriding operators and produce a syntax error when
# used in the context of an actual expression
operator_type (
    '_add_' | '_subtract_' | '_multiply_' | '_divide_' | '_modulo_' | '_power_' |
    '_less_than_' | '_more_than_' | '_less_or_equal_' | '_more_or_equal_' |
    '_equal_' | '_not_equal_' |
    '_assign_' | '_add_equal_' | '_subtract_equal_' | '_multiply_equal_' |
    '_divide_equal_' | '_modulo_equal_' |
    '_unary_not_' | '_unary_negate_' |
    '_and_' | '_or_' |
    '_create_' | '_destroy_'
)

# Content of a type definition.
type_element (
    scope_operator |
    data_name_decl |
    ( func_identifier func_parameters func_body? )
)

type_parameter (
    scope_operator? compound_identifier ('as' IDENTIFIER)?
)

# Define a structure.
type_definition (
    'type' IDENTIFIER ( '(' ( type_parameter (',' type_parameter)* )? ')' )?
    '{' type_element (type_element)* '}'
)

# Define a namespace. Anything that can be placed in a program that go in a
# namespace, including a namespace.
namespace_definition (
    'namespace' IDENTIFIER '{' program_item+ '}'
)

# Top level expression. Lowest precedence to highest. No left recursion
# is allowed.
expression (
    expr_and (('|' | 'or') expr_and)*
)

expr_and (
    expr_equal (('&' | 'and') expr_equal)*
)

expr_equal (
    ( expr_magnitude (('==' | 'equ') expr_magnitude)* ) |
    ( expr_magnitude (('!=' | 'nequ') expr_magnitude)* )
)

expr_magnitude (
    ( expr_sum (('>' | 'gt') expr_sum)* ) |
    ( expr_sum (('<' | 'lt') expr_sum)* ) |
    ( expr_sum (('>=' | 'gte') expr_sum)* ) |
    ( expr_sum (('>=' | 'lte') expr_sum)* )
)

expr_sum (
    (expr_product ('+' expr_product)* ) |
    (expr_product ('-' expr_product)* )
)

expr_product (
    (expr_product ('*' expr_product)* ) |
    (expr_product ('/' expr_product)* ) |
    (expr_product ('%' expr_product)* )
)

expr_power (
    expr_unary_neg ('^' expr_unary_neg)*
)

expr_unary_neg (
    ('-' expr_unary_not)* |
    expr_primary
)

expr_unary_not (
    (('!' | 'not') expr_primary)* |
    expr_primary
)

expr_primary (
    ('(' expression ')') |
    INT_LITERAL |
    FLOAT_LITERAL |
    BOOL_LITERAL |
    formatted_string |
    compound_reference |
    cast_clause
)

cast_clause (
    type_name '<' compound_reference '>'
)

compound_reference_item (
    IDENTIFIER |
    array_reference |
    func_reference
)

array_ref_index (
    expression |
    STRING_LITERAL
)

array_reference (
    IDENTIFIER '[' array_ref_index ']' ('[' array_ref_index ']')*
)

func_reference (
    IDENTIFIER '(' (expression (',' expression)* )? ')'
)

# Accessing a object that has already been defined.
compound_reference (
    compound_reference_item ('.' compound_reference_item)*
)

array_init_item (
    expression |
    (STRING_LITERAL ':' expression) |
    array_initializer
)

array_initializer (
    '{' array_init_item (',' array_init_item )* '}'
)

assignment (
    ( compound_reference '=' (expression | array_initializer) ) |
    ( compound_reference '+=' expression ) |
    ( compound_reference '-=' expression ) |
    ( compound_reference '*=' expression ) |
    ( compound_reference '/=' expression ) |
    ( compound_reference '%=' expression )
)

func_body_element (
    data_declaration |
    data_decl_assignment |
    compound_reference |
    assignment |
    if_statement |
    while_statement |
    do_statement |
    for_statement |
    return_statement |
    exit_statement |
    raise_statement |
    exception_block |
    INLINE
)

func_body (
    '{' (func_body_element | func_body)* '}'
)

loop_body_element (
    func_body_element |
    'continue' |
    'break'
)

loop_body (
    '{' (loop_body_element | loop_body)* '}'
)

if_statement (
    if_clause else_clause* final_else?
)

# If the expression is !zero then the func_body is run
if_clause (
    'if' '(' expression ')' func_body
)

else_clause (
    'else' '(' expression ')' func_body
)

# Absent expression is always "true"
final_else (
    'else' ( '(' expression? ')' )? func_body
)

# Note that an empty or absent expression is equivalent to while(1)
while_clause (
    'while' ( '(' expression? ')' )?
)

while_statement (
    while_clause loop_body
)

do_statement (
    'do' loop_body while_clause
)

# If the identifier is present then the result of the expression is assigned
# to it and then, if the result is not zero, then the loop is executed and
# the value is in the identifier. Otherwise the identifier goes out of scope.
# If the expression is absent then it's equivalent to while(1).
for_statement (
    'for' ( '(' (type_name? IDENTIFIER 'in')? expression ')' )? loop_body
)

# If the expression is present then it's apparent type must match the return
# type of the funciton that it's defined in.
return_statement (
    'return' ( '(' expression? ')')?
)

# Expression must evaluate to an int as it's apparent type.
exit_statement (
    'exit' '(' expression ')'
)

# This is intended to be used to handle errors. The identifier must be
# unique in the program namespace. The expression is passed back to the
# exception handler as needed.
raise_statement (
    'raise' '(' IDENTIFIER ',' (expression | 'nothing') ')'
)

# At least one except clause is required
exception_block (
    try_clause except_clause+ final_clause?
)

try_clause (
    'try' func_body
)

# The first identifier is a globally unique name that identifies the
# exception to be handled. The second identifier is the name of the
# data to pass back to the handler. The type must match the apparent
# type of the expression that was given in the raise() statement.
except_clause (
    'except' '(' IDENTIFIER ',' (type_name | 'nothing') IDENTIFIER ')' func_body
)

# The final clause catches all exceptions.
final_clause (
    'final' ('(' (type_name IDENTIFIER)? ')')? func_body
)

