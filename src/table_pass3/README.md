# table pass3

In this pass the table is simplified by deleting nodes that have the following attributes:
* Has no terminal symbol.
* Has no code connected to it.
* Has no references, such as zero or more.
* Has no not-match link. This happens when there is no alternative match.
