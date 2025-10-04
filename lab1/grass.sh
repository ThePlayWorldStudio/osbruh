#!/bin/bash

if [ $# -ne 5 ]; then
	echo "expect 5 args"
	exit 1
fi

min_size=$1
max_size=$2
user=$3
dir=$4
outfile=$5

if ! [[ "$min_size" =~ ^[0-9]+$ ]] || ! [[ "$max_size" =~ ^[0-9]+$ ]]; then
	echo "args 1 and 2 must be numbers"
	exit 1
fi

counter=0
> "$outfile"

while read owner size path; do
	(( counter++ ))

	if [[ "$owner" = "$user" ]] && [[ "$min_size" -le "$size" ]] && [[ "$size" -le "$max_size" ]]; then
		filename=$(basename "$path")
		echo "$path, $filename, $size" >> "$outfile"
	fi
done < <(find "$dir" -type f -printf "%u %s %p\n" 2>/dev/null)

echo "$counter"

exit 0

