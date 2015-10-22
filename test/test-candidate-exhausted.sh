#!/bin/bash
make col-clean
./index-math.stackexchange.com.sh.ln './raw-test-candidate-exhausted'
./search.out.ln -q '-b \pm \sqrt{b^2 - 4ac}'
