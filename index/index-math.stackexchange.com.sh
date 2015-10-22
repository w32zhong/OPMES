#!/bin/bash
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'the script to index math.stackexchange.com.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <raw collection directory>"
	echo ''
	echo 'OUTPUT:'
	echo "./col directory and *.db files."
	exit
fi

dir="${1}"
index='./index.out'
bad_log='bad-parsing.log'

[ $# -ne 1 ] && echo "bad arg." && exit
[ ! -d ${dir} ] && echo "not dir." && exit
[ ! -e ${index} ] && echo "index command not found." && exit

echo 'calculating total files...'
total=`find ${dir} -maxdepth 1 -type f | wc -l`
cnt=0
echo "total = ${total}"

# handle one raw crawled file
function handle ()
{
	let 'cnt = cnt + 1'
	filename=`basename ${1}`
	echo -n "[ ${filename} ] indexing progress: ${cnt}/${2}; "

	num=`echo "$1" | grep -P -o '(?<=summary-)[0-9]+'`
	url="http://math.stackexchange.com/questions/$num"
	# echo "(url = $url)"

	total_lines=`cat $1 | wc -l`
	bad_parse=0
	while read -r tex 
	do 
		${index} -f "${tex}" -u "${url}" > /dev/null 2>&1

		if [ $? -ne 0 ]
		then
			let 'bad_parse = bad_parse + 1'
			echo "${tex} \`$1'" >> ${bad_log}
		fi
	done < $1

	echo -n "bad lines: "
	if [ ${bad_parse} -eq 0 ] 
	then
		tput setaf 2 # green color
		echo -n "${bad_parse}/${total_lines}"
		tput sgr0
	else	
		tput setaf 1 # red color
		echo -n "${bad_parse}/${total_lines}"
		tput sgr0
	fi
	echo '.'
}
 
> ${bad_log}
find $dir -type f | while read f; do handle ${f} ${total}; done
