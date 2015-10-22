LDLIBS = -Xlinker "-(" -lcurl -lsearch -lindex -lfl -lparser -lcommon -lrt \
-ltokyocabinet -lpthread -lbz2 -lz -lm -lcrypto -lssl -Xlinker "-)"

include rules.mk
include config.mk

CFLAGS += -D_GNU_SOURCE # for readline()

WEB_RES_DIR = $(WEB_DIR)/res
WEB_CGI_DIR = $(WEB_DIR)/cgi

.PHONY: install uninstall permission

all: helloworld.cgi search.cgi show-tex-list.cgi

helloworld.cgi: helloworld.o 
	$(LINK)

search.cgi: main.o
	$(LINK)

show-tex-list.cgi: show-tex-list.o
	$(LINK)

install: 
	mkdir -p $(WEB_DIR)
	mkdir -p $(WEB_RES_DIR)
	mkdir -p $(WEB_CGI_DIR)
	cp res/index.html $(WEB_RES_DIR)
	cp res/about.html $(WEB_RES_DIR)
	cp res/mathman.png $(WEB_RES_DIR)
	cp helloworld.cgi $(WEB_CGI_DIR)
	cp search.cgi $(WEB_CGI_DIR)
	cp show-tex-list.cgi $(WEB_CGI_DIR)
	cp test-search.sh $(WEB_CGI_DIR)
	cp -r cat $(WEB_CGI_DIR)
	make permission

permission:
	find $(WEB_DIR) -type d | xargs chmod 755
	find $(WEB_DIR) -type f | xargs chmod 644
	find $(WEB_DIR) -name '*.sh' -o -name '*.cgi' | xargs chmod 755

uninstall:
	rm -rf $(WEB_DIR)

clean: common-clean
	rm -f *.cgi
