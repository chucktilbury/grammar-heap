# This is a special make file that creates the PNG files for the readme.

TARGETS	= group.png match.png oom.png zom.png zoo.png or.png

all: $(TARGETS)

group.png: group.dot
	dot -Tpng -o group.png group.dot

match.png: match.dot
	dot -Tpng -o match.png match.dot

oom.png: one-or-more.dot
	dot -Tpng -o oom.png one-or-more.dot

zom.png: zero-or-more.dot
	dot -Tpng -o zom.png zero-or-more.dot

zoo.png: zero-or-one.dot
	dot -Tpng -o zoo.png zero-or-one.dot

or.png: or.dot
	dot -Tpng -o or.png or.dot

