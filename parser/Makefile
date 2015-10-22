LDLIBS = -lfl -lcommon -lreadline -lncurses
include rules.mk

CFLAGS += -D_GNU_SOURCE # for open_memstream()

all: libparser.a parser.out hello/parser-hello.out test-tex-tr.out 

HEAD_REQ_OBJS:=y.tab.o lex.yy.o tex-tr.o test-tex-tr.o gen-trans.o

$(HEAD_REQ_OBJS): enum-symbol.h gen-trans.c

gen-trans.c: enum-symbol.h enum-token.h trans.c
	./gen-trans-tab.sh $^ 

parser.out: main.o libparser.a 
	$(LINK)

enum-symbol.h gen-lexer.l: lexer.l symbol.h 
	./gen-symbol-tab.sh $^
	rm -f enum-symbol.h
	ln -s gen-symbol.h enum-symbol.h

lex.yy.c: gen-lexer.l y.tab.h
	$(LEX)

y.tab.h y.tab.c y.dot: grammar.y
	$(YACC)

hello/parser-hello.out:
	@make -C hello

libparser.a: y.tab.o lex.yy.o tex-tr.o gen-trans.o parser-util.o
	$(AR)

test-tex-tr.out: test-tex-tr.o libparser.a 
	$(LINK)

clean: common-clean
	rm -f *.tab.[ch]
	rm -f lex.yy.c
	rm -f *.dot
	rm -f enum-symbol.h
	rm -f gen-*.l
	rm -f gen-*.h
	rm -f gen-*.c
	@make -C hello clean
