#!/bin/bash
log_name="${0}.log.output"
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'pick a random file from a directory.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <dir>"
	echo ''
	echo 'OUTPUT:'
	echo "the random file name"
	echo "$log_name"
	exit
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
[ $# -ne 1 ] && echo "bad arg." && exit
[ ! -d ${1} ] && echo "not dir." && exit
cd ${DIR}/${1}
echo "pick random file @ `pwd`"
file=`ls | shuf -n 1`
echo ${file} | tee -a ${DIR}/${log_name} 
