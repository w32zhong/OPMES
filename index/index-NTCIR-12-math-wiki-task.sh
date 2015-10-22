#!/bin/bash
if [ $# -eq 1 ] && [ $1 == '-h' ]
then
	echo 'DESCRIPTION:'
	echo 'the script to index NTCIR 12 math wiki task.'
	echo ''
	echo 'SYNOPSIS:'
	echo "$0 <html collection directory>"
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
	file="${1}"	
	basename=`basename "$file"`
	url="file://$file"
	total_tex_lines=0
	bad_parse=0

	let 'cnt = cnt + 1'
	echo -n "[ ${basename} ] indexing progress: ${cnt}/${total}; "

	while read -r -d $'\n' tex 
	do
		echo "$tex" | sed -e 's/%&#10;/ /g' -e 's/&#10;/ /g' > /tmp/NTCIR.tmp # filter out newline HTML entity
		tex=`php -r "echo html_entity_decode(file_get_contents('/tmp/NTCIR.tmp'));"`
		# echo "${tex}" # debug
		${index} -f "${tex}" -u "${url}" > /dev/null 2>&1

		if [ $? -ne 0 ]
		then
			let 'bad_parse = bad_parse + 1'
			echo "${tex} \`$1'" >> ${bad_log}
		fi
		let 'total_tex_lines = total_tex_lines + 1'
	done <<-EOF
	$(cat "$file" | grep '<math' | grep -Po '(?<=alttext=").*?(?=")')
	EOF

	echo -n "bad lines: "
	if [ ${bad_parse} -eq 0 ] 
	then
		tput setaf 2 # green color
		echo -n "${bad_parse}/${total_tex_lines}"
		tput sgr0
	else	
		tput setaf 1 # red color
		echo -n "${bad_parse}/${total_tex_lines}"
		tput sgr0
	fi
	echo '.'
}
 
> ${bad_log}
find $dir -type f | while read f; do handle ${f}; done
