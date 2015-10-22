LDLIBS = -lfl -lcommon
include rules.mk

all: parser-hello.out

parser-hello.out: y.tab.o lex.yy.o 
	$(LINK)

lex.yy.c: hello.l y.tab.h 
	$(LEX)
	
y.tab.h y.tab.c y.dot: hello.y 
	$(YACC)

automation.ps: y.dot
	$(DOT)

clean: common-clean
	rm -f *.tab.[ch]
	rm -f lex.yy.c
	rm -f *.dot
	rm -f *.log
	rm -f *.ps
