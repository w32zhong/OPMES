LDLIBS = -lhello
include rules.mk

CFLAGS += -DPRI_VER

all: hello

hello: main.o
	$(LINK)

clean: common-clean
	rm -f hello 
