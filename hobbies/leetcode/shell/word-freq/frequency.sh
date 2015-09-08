#!/usr/bin/env bash
declare -A frequency

while read line; do
    for word in $line; do
        frequency[$word]=$((frequency[$word] + 1))
    done
done < words.txt

for word in ${!frequency[@]}; do
    echo $word ${frequency[$word]}
done | awk '{ print $2 " " $1; }' | sort -nr | awk '{ print $2 " " $1; }'
