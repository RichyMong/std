declare -i lineno

lineno=0

while read line; do
    lineno=$((lineno + 1))
    if [ $lineno -eq 10 ]; then
        echo $line
        break
    fi
done < file.txt

# will print the last line if the file contains less than 10 lines.
head -n 10 file.txt | tail -n 1

sed -n '10p' file.txt

grep -n ".*" file.txt | grep "^10:" | cut -d: -f2-
