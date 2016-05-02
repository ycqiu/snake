#!/bin/bash

succ=0
fail=0

for(( i = 0; i < 100; ++i ))
do
	./hello 
	res=$?
	echo $res >> res
	if [[ $res -eq 1 ]]
	then
		succ=$[$succ+1] 
	else
		fail=$[$fail+1]
	fi
done


echo "fail: $fail"
echo "succ: $succ"
