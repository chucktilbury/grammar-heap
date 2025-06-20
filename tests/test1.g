
# a simple terminal and non-terminal in a single rule
%pretext {}

simple
    : something_else {}
    | POOTS {}
    ;

something_else
    : IDENTIFIER {}
    | 'asdf' {}
    ;

%pretext {}
