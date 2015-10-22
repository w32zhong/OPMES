.PHONY: all clean common-clean tags-clean

CC = @ echo '[compile]' $< && gcc 

ifeq ($(LDLIBS), ) 
	LD = @ echo '[link] $@' && gcc
else
	LD = @ echo '[link with $(LDLIBS)] $@' && gcc
endif

DEP = @ gcc -MM
LINK = $(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
AR = @ echo '[ar]' $@ && ar rcs $@ $^

CFLAGS = -Wall -Wno-unused-function -std=c99 -pedantic

LEX = @ echo '[lex]' $< && flex $<
YACC = @ echo '[yacc]' $< && bison -v -d --report=itemset -g $< -o y.tab.c
DOT = dot -Tps $< -o $@

SRC_LIST = srclist.txt
SRC := $(shell test -e ${SRC_LIST} && cat ${SRC_LIST})

FIND=@ find . -type d \( -name 'col-*' -o -name 'col' -o -name 'release' -o -path './.git' \) -prune -o

PROJ_DIR=$(shell dirname '$(shell readlink -f '$(CURDIR)/rules.mk')')

DEP_LINKS=$(addprefix ./dep, $(LDLIBS:=.mk))

-include $(DEP_LINKS)

all: 
	@echo '[done]'

find-%:
	$(FIND) -name '$*' -print

find-test:
	$(FIND) -name '*' -print > test-find.tmp

include $(wildcard *.d)

%.o: %.c
	$(CC) $(CFLAGS) $*.c -c -o $@
	$(DEP) -MT $@ $(CFLAGS) $*.c -o $*.d

common-clean:
	@ echo [common clean]
	$(FIND) -type f \( -name 'Session.vim' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.output' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.[od]' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.tmp' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.gch' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.pyc' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.out' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.log' \) -print | xargs rm -f
	$(FIND) -type l \( -name '*.ln' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.so' \) -print | xargs rm -f
	$(FIND) -type f \( -name '*.a' \) -print | xargs rm -f

tags: $(SRC) 
	@ echo '[tags]'
	$(FIND) -name '*.[hcly]' -print > $(SRC_LIST)
	@ if command -v ctags; then \
		ctags --langmap=c:.c.y -L $(SRC_LIST); \
	else \
		echo 'no ctags command.'; \
	fi
	
tags-clean:
	@echo '[tags clean]'
	@$(FIND) -type f \( -name 'tags' \) -print | xargs rm -f
	@rm -f $(SRC_LIST)	

define mk-symbol-link
@ if [ ! -h $2 ]; then \
	echo [symbol link $$(cd `dirname $1` && pwd)/`basename $1` to $2]; \
	ln -s $$(cd `dirname $1` && pwd)/`basename $1` $2; \
fi; 
endef
