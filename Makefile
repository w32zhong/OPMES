include rules.mk

include web/config.mk
CGI_PATH = $(WEB_DIR)/cgi

MODULES := hello crawler common parser index search web test

.PHONY: test libhello libhello-clean $(MODULES) install all-rules.mk

all: tags all-rules.mk libhello $(MODULES)

clean: tags-clean libhello-clean $(MODULES:=-clean) clean-demo
	@ echo '[clean deplinks]'
	$(FIND) -name 'dep-*.mk' -print | xargs rm -f
	make clean-rules.mk

clean-demo:
	rm -rf col col-raw-math.stackexchange.com *.log *.db *.out

demo: all
	python2 ./crawler/crawler-math.stackexchange.com.py -p 314159 # crawl http://math.stackexchange.com/questions/314159
	ln -s ./index/index.out
	./index/index-math.stackexchange.com.sh col-raw-math.stackexchange.com/
	find ./col
	./search/search.out -n -q '1/2 (n-1)!'

all-rules.mk:
	make common/rules.mk
	make hello/rules.mk
	make parser/rules.mk
	make parser/hello/rules.mk
	make index/rules.mk
	make search/rules.mk
	make web/rules.mk

clean-rules.mk:
	$(FIND) -type l -name 'rules.mk' -print | xargs rm -f

%-deplinks: 
	@ find dep/ -type f -exec basename {} \; > tmp
	@ while read link; \
	do \
		if [ ! -e $*/dep-$${link} ]; then \
			ln -s ${PROJ_DIR}/dep/$${link} $*/dep-$${link}; \
		fi; \
	done < tmp
	@ rm -f tmp

libhello:
	make -C hello/elsewhere

%/rules.mk:
	@ echo [$@]
	@ ln -s ../rules.mk $*/rules.mk

libhello-clean:
	make -C hello/elsewhere clean 

hello: libhello hello-deplinks
	make -C hello

crawler:
	make -C crawler

common:
	make -C common 

parser: common parser-deplinks parser/hello-deplinks 
	make -C parser 

index: common parser index-deplinks
	make -C index 

search: common parser index search-deplinks 
	make -C search

web: common parser index search web-deplinks
	make -C web 

test: index search
	$(call mk-symbol-link, ./parser/parser.out, ./test/parser.out.ln)
	$(call mk-symbol-link, ./index/index.out, ./test/index.out)
	$(call mk-symbol-link, ./index/probe.out, ./test/probe.out.ln)
	$(call mk-symbol-link, ./index/index-math.stackexchange.com.sh, ./test/index-math.stackexchange.com.sh.ln)
	$(call mk-symbol-link, ./search/search.out, ./test/search.out.ln)

install: search ./release
	@ touch /root/permission_test
	make -C web install
	$(call mk-symbol-link, release/formula.db, $(CGI_PATH)/formula.db)
	$(call mk-symbol-link, release/col, $(CGI_PATH)/col)
	$(call mk-symbol-link, search/search.out, $(CGI_PATH)/search.out)
	make -C web permission 
	
uninstall:
	@ touch /root/permission_test
	make -C web uninstall

%-clean: all-rules.mk
	make clean -C $*
