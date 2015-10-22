LDLIBS = -lrt # clock_gettime() and clock_nanosleep()
include rules.mk
CFLAGS += -D_XOPEN_SOURCE=600 # clock_gettime() and clock_nanosleep() 

TEST_BINS := $(addprefix test-, list tree trace timer)
TEST_BINS := $(addsuffix .out, $(TEST_BINS))

all: libcommon.a $(TEST_BINS)

libcommon.a: tree.o trace.o trace-unfree.o timer.o
	$(AR)	

test-list.out: test-list.o
	$(LINK)

test-tree.out: test-tree.o tree.o
	$(LINK)

test-trace.out: test-trace.o trace.o trace-unfree.o
	$(LINK)

test-timer.out: test-timer.o timer.o trace.o trace-unfree.o
	$(LINK)

clean: common-clean
	rm -f *.log
