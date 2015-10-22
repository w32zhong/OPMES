#!/bin/bash
output_name="${0}.output"
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'get the distribution of tex command in a raw collection.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <dir>"
	echo ''
	echo 'OUTPUT:'
	echo "$output_name"
	exit
fi

[ $# -ne 1 ] && echo "bad arg." && exit
[ ! -d ${1} ] && echo "not dir." && exit

dir=${1}

> tmp
find $dir -type f -exec \
	bash -c './cmd-grep.sh --all-commands {} >> tmp' \;
sort tmp | uniq -c > ${output_name} 
rm tmp
