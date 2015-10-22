LDLIBS = -Xlinker "-(" -lindex -lfl -lparser -lcommon -lrt \
-ltokyocabinet -lpthread -lbz2 -lz -lm -lcrypto -lssl -Xlinker "-)"

include rules.mk

CFLAGS += -D_DEFAULT_SOURCE # for readdir() and then ent->d_type & DT_DIR

TEST_BINS := $(addprefix test-, score dirmerge posmerge minheap)
TEST_BINS := $(addsuffix .out, $(TEST_BINS))

all: search.out $(TEST_BINS)

test-score.out: score.o test-score.o
	$(LINK)

test-dirmerge.out: dirmerge.o test-dirmerge.o
	$(LINK)

test-posmerge.out: posmerge.o dirmerge.o test-posmerge.o
	$(LINK)

test-minheap.out: minheap.o test-minheap.o
	$(LINK)

libsearch.a: search.o score.o rank.o dirmerge.o posmerge.o minheap.o
	$(AR)

search.out: main.o libsearch.a 
	$(LINK)

clean: common-clean
	rm -f *.bin
