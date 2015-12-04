#!/bin/bash
#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=number of factors
#

#declare data=(100.log 250.log 500.log 1000.log 2000.log 4000.log 100_h4.log 250_h4.log 500_h4.log 1000_h4.log 2000_h4.log 4000_h4.log 100_h8.log 250_h8.log 500_h8.log 1000_h8.log 2000_h8.log 4000_h8.log)
#500_replacedirect 500_replacefitness 
declare samples=(500-baseline)
declare dataset=(7)
declare pm=(0.02702 0.1 0.3 0.5)
maxgen=25

n=-1
for (( i = 0; i < 1; i++ )) #samples
do
	for (( p = 0; p < 1; p++ ))      ### denotes the dataset ###
	do
		for (( k = 0; k < 1; k++ ))      ### denotes the mutation probability ###
		  do  
			  for (( j = 1; j <= $maxgen; j++ ))      ### denotes the generations ###
			     do
				#printf "Sample Size: ${samples[$i]} $m GA run ${dataset[$p]} hidden variables ${pm[$k]} mutation probability \t\n " >> ./${samples[$i]}/${dataset[$p]}.sorted.log
			  	for(( m = 0; m < 25; m++))  ### denotes the no. of GA runs ###
			     	do			
					num=$(expr $j*20| bc)
					#echo $j $num
					#head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | awk '{print $3}'|sort | head -1
					val=$(head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | awk '{print $3}'|sort | head -1)
					maxval=$(expr $n*$val| bc)
					#echo $maxval
					avgiter=$(head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | grep $maxval | awk '{print $2}' | awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1)
					#echo $avgiter
					echo "$j $m $val  $avgiter" >> ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.log
					#rm -r ./$1/${samples[$i]}/output/sorted.log
			     	done		
				num1=$(expr $j*25| bc)
				num2=$(expr \($j-1\)*25+1| bc)		
				#echo $num1 $num2
				avgll=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.log | tail -$((($num1-$num2)+1)) | awk '{print $3}'| awk '{sum += $1;total += 1;printf"%.3f\n", sum/total}'|tail -1)
				avgit=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.log | tail -$((($num1-$num2)+1)) | awk '{print $4}'| awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1 )
				echo "$avgll  $avgit" >> baseline_em_output.log	
			     done
				
		done
	done
done

