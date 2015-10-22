LDLIBS =-lparser -lfl -lcommon -ltokyocabinet -lpthread -lbz2 -lz -lcrypto -lssl -lm
include rules.mk

CFLAGS += -D_GNU_SOURCE # for open_memstream()

all: index.out probe.out libindex.a test-write.out test-read.out

index.out: index.o libindex.a
	$(LINK)

probe.out: probe.o libindex.a
	$(LINK)

libindex.a: db-wraper.o index-util.o
	$(AR)

test-write.out: test-write.o libindex.a
	$(LINK)

test-read.out: test-read.o libindex.a
	$(LINK)

clean: common-clean

col-clean:
	rm -f *.db
	rm -f *.log
	rm -rf col
