#!/bin/bash
#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=number of factors
#

#declare data=(100.log 250.log 500.log 1000.log 2000.log 4000.log 100_h4.log 250_h4.log 500_h4.log 1000_h4.log 2000_h4.log 4000_h4.log 100_h8.log 250_h8.log 500_h8.log 1000_h8.log 2000_h8.log 4000_h8.log)

declare samples=(500)
declare dataset=(7)
declare pm=(0.001)
# 0.002 0.005 0.1 0.3 0.5)
maxgen=5

n=-1
for (( i = 0; i < 1; i++ )) #samples
do
	for (( p = 0; p < 1; p++ ))      ### denotes the dataset ###
	do
	  for (( k = 0; k < 1; k++ ))      ### denotes the mutation probability ###
	     do
		printf "Sample Size: ${samples[$i]} ${pm[$p]} hidden variables ${pm[$k]} mutation probability \t\n " >> ./${samples[$i]}/${dataset[$p]}/${dataset[$p]}.sorted.log
	  	for (( j = 1; j <= $maxgen; j++ ))      ### denotes the mutation probability ###
	     	do			
			num=$(expr $j*10| bc)
			#echo $j $num
			head -$num ./${samples[$i]}/${dataset[$p]}/${pm[$k]}/${pm[$k]}.log | awk '{print $7}'|sort | head -1
			val=$(head -$num ./${samples[$i]}/${dataset[$p]}/${pm[$k]}/${pm[$k]}.log | awk '{print $7}'|sort | head -1)
			maxval=$(expr $n*$val| bc)
			echo $maxval
			avgiter=$(head -$num ./${samples[$i]}/${dataset[$p]}/${pm[$k]}/${pm[$k]}.log | grep $maxval | awk '{print $6}' | awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1)
			echo $avgiter
			echo "$val  $avgiter" >> ./${samples[$i]}/${dataset[$p]}/${dataset[$p]}.sorted.log
			#rm -r ./$1/${samples[$i]}/output/sorted.log
	     	done
		#avgll=$(head -200 ./$1/${samples[$i]}/output/$p.log | awk '{print $4}'| awk '{sum += $1;total += 1;printf"%.3f\n", sum/total}'|tail -1)
		#avgit=$(head -200 ./$1/${samples[$i]}/output/$p.log | awk '{print $3}'| awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1 )
		#echo "$avgll  $avgit" >> ./$1/${samples[$i]}.sorted.log
	     done	
	done
done
#paste ./$1/100.sorted.log ./$1/250.sorted.log ./$1/500.sorted.log ./$1/1000.sorted.log ./$1/2000.sorted.log ./$1/4000.sorted.log > ./$1/sorted.log
#rm -r ./$1/100.sorted.log ./$1/250.sorted.log ./$1/500.sorted.log ./$1/1000.sorted.log ./$1/2000.sorted.log ./$1/4000.sorted.log
