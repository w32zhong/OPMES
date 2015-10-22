#!/bin/bash
make col-clean
./index-math.stackexchange.com.sh.ln './raw-test-partial'
./search.out.ln -q 'W(2,k)>2^{k}/k^{\varepsilon}'
