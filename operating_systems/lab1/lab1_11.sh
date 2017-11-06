#!/bin/bash

# 1
sum=0
for file in $(find . -mindepth 2 -maxdepth 2 -type f ! -perm /222 -perm /111 -perm -444); do
  lines="$(wc -l $file | awk '{print $1}')"
  let sum+=$lines
done

part1="$(printf '%03d' $sum | tail -c 3)"



# 2
length_sum=0
for file in $(find . -mindepth 3 -maxdepth 3 -type f); do
  longest_string="$(wc -L $file | awk '{print $1}')"
  let length_sum+=$longest_string
done

first_md5="$(printf '%s' $length_sum | md5sum | awk '{print $1}')"
list_size_files="$(ls -l -r --sort=extension | grep ^- | awk '{print $5}')"
second_md5="$(printf '%s' '$list_size_files' | md5sum | awk '{print $1}')"

md5_sum="$(printf '%s\n%s' '$first_md5' '$second_md5' | sort | md5sum | awk '{print $1}')"

part2="$(echo ${md5_sum:1:6})"


#3
export LC_ALL=en_US.UTF-8
name_month="$(date +%B)"

biggest_file="$(find . -maxdepth 1 -type f -iname '*'$name_month'*' -printf '%s\t%p\n'  | sort -n | tail -1 | awk '{print $2}')"
smallest_file="$(find . -mindepth 3 -maxdepth 3 -type f -printf '%s\t%p\n' | sort -n | awk '{print $2}')"

smallest=""
for file in $smallest_file; do
  words="$(wc -w $file | awk '{print $1}')"
  lines="$(wc -l $file | awk '{print $1}')"

  let lines*=3
  if [[ "$words" -eq "$lines" ]]; then
    smallest=$file
    break
  fi
done
  
if [[ "$smallest" = "" ]] || [[ "$biggest_file" = "" ]]; then
  part3='b'
else
  first_content="$(cat "$biggest_file" | sort | uniq -u)"
  second_content="$(cat "$smallest" | sort)"
  part3='1'
  if [ "$first_content" = "$second_content" ]; then
    part3='0'
  fi
fi




#4
sumr=0
curlvl=1
while [ 1 ]
do
  lvlWF="$(find . -mindepth $curlvl -type f)"
  if [ "$lvlWF" = "" ]; then
    break
  fi
  lvlList="$(find . -mindepth $curlvl -maxdepth $curlvl -type f -printf '%s\t%p\n' | sort -n | awk '{print $2}')"
  if [ "$lvlList" = "" ]; then
    let curlvl+=1
    continue
  fi

  int_file="$(printf '%s\n' $lvlList | head -3 | tail -1)"

  filesize=$(stat -c%s "$int_file")
  let sumr+=$filesize
  let curlvl+=1
done
part4="$(printf '%04d' $sumr | tail -c 4)"
echo $part1$part2$part3$part4

