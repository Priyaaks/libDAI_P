#!/bin/bash
num2=1
n=$(($1 - num2))
for (( i = 0; i <= $n; i++ )) #samples
do
    j=$((i + num2))
    name=$name" ./new/$j.log"
    name1="$i-$j.log"
    name2="$j.log"
	./examples/computeDistance ./new/_$i.log ./new/_$j.log >>./new/$name1
    sort -k3,3 ./new/$name1 >./new/$name2
    echo $name
done
paste $name>./new/result.log
#paste ./$1/100.sorted.log ./$1/250.sorted.log ./$1/500.sorted.log ./$1/1000.sorted.log ./$1/2000.sorted.log ./$1/4000.sorted.log > ./$1/sorted.log
#rm -r ./$1/100.sorted.log ./$1/250.sorted.log ./$1/500.sorted.log ./$1/1000.sorted.log ./$1/2000.sorted.log ./$1/4000.sorted.log
