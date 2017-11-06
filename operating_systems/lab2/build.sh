#!/bin/bash
# DO NOT MODIFY THIS FILE!
cd "$(dirname "$0")"
res=0
g++ -Wall -Werror main.cpp -o main.out -std=c++11
if [ "$?" -ne "0" ]; then
  res=1
fi

exit $res
