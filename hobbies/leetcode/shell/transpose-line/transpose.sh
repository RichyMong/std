#!/usr/bin/env bash
declare -a columns

while read line; do
    fieldno=0
    for word in $line; do
        columns[$fieldno]="${columns[$fieldno]} ${word}"
        fieldno=$(($fieldno + 1))
    done
done < file.txt

for ((i = 0; i < ${#columns[*]}; i++)); do
    echo ${columns[i]}
done
