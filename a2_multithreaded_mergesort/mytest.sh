#!/bin/bash
cnt=`shuf -i 64-128 -n 1`
for f in {1..10}
do
  #printf "%3s: " $cnt
  echo "amt is {$cnt} " > mytest.out
  ./sort -m $cnt > mytest.out
done


