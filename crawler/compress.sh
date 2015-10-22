#!/bin/bash
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'a short cut to compress raw collection.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <dir>"
	echo ''
	echo 'OUTPUT:'
	echo "compressed <dir>"
	exit
fi

[ $# -ne 1 ] && echo "bad arg." && exit
[ ! -d ${1} ] && echo "not dir." && exit

dir=${1}
compress_name=`basename "${dir}"`
compress_name=${compress_name}.tar.bz2
compress_name=`echo ${compress_name} | grep -Po '(?<=raw-).*'`

if [ ! -e ${compress_name} ];
then
	tar -cjf ${compress_name} ${dir}
else
	echo "${compress_name} already exists."
fi;
