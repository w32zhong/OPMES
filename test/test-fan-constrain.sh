#!/bin/bash
make col-clean
./index-math.stackexchange.com.sh.ln './raw-test-fan-constrain'
./search.out.ln -q 'a+b+c+c'
