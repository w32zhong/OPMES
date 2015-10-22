#!/bin/bash
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'grep one/all tex command(s) from raw collection file(s).'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 --all-commands <file0> <file1> ..."
	echo "$0 --command <tex command> <file0> <file1>..."
	echo ''
	echo 'OUTPUT:'
	echo "stdout"
	echo ''
	echo 'EXAMPLE:'
	echo "$0 --all-commands ./raw/*"
	echo "$0 --command frac ./raw/*"
	exit
fi

[ $# -eq 0 ] && echo "bad arg." && exit

if [ $1 == '--all-commands' ]
then
	grep -oP '(?<!\\)\\[a-zA-Z]+' ${@:2}
elif [ $1 == '--command' ]
then
	grep --color -P "(?<!\\\\)\\\\${2}[^a-z^A-Z]+" ${@:3}
else
	echo 'bad arg.'
fi
