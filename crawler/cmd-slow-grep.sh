#!/bin/bash
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'slowly grep a tex command in a large volume raw collection.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <tex command> <dir>..."
	echo ''
	echo 'OUTPUT:'
	echo "stdout"
	exit
fi

[ $# -ne 2 ] && echo "bad arg." && exit
[ ! -d ${2} ] && echo "${2} is not dir." && exit

find ${2} -type f | xargs -I % bash -c "./cmd-grep.sh --command ${1} % && echo '(%)'; sleep 0.2"
