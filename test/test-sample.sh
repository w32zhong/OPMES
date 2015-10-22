#!/bin/bash
make col-clean
./index-math.stackexchange.com.sh.ln './raw-test-sample'
./search.out.ln -n -q '\operatorname{E} \left[ \sup_{t \geq 0} \left| \int_t^{t+h} X_s \, \mathrm{d} B_s \right|^p \right]'
