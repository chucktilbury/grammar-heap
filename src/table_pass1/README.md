# table_pass1

This creates the raw state table and populates it with the table items. When the create_table_pass1() function returns the raw_table_t has been allocated and initialized. Each item in the raw table is a non-terminal symbol with its associated nodes in a list with pointers to the match and no-match nodes. This implements the loops and the interface to other nodes.

