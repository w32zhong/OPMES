#!/bin/bash
make col-clean
./index-math.stackexchange.com.sh.ln './raw-test-name-match'
./search.out.ln -q 'a + \frac a b'
