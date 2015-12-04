#!/bin/bash
#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=number of factors
#

#declare data=(100.log 250.log 500.log 1000.log 2000.log 4000.log 100_h4.log 250_h4.log 500_h4.log 1000_h4.log 2000_h4.log 4000_h4.log 100_h8.log 250_h8.log 500_h8.log 1000_h8.log 2000_h8.log 4000_h8.log)
#500_replacedirect 500_replacefitness 
declare samples=(500)
declare dataset=(19)
declare pm=(0.02702 0.3 0.5)
maxgen=25

n=-1
for (( i = 0; i < 1; i++ )) #samples
do
	for (( p = 0; p < 1; p++ ))      ### denotes the dataset ###
	do
		for (( k = 0; k < 3; k++ ))      ### denotes the mutation probability ###
		  do  
			 printf "${pm[$k]} mutation probability \t\n " >>./${samples[$i]}/${pm[$k]}.sorted.log
			  	
			  for (( j = 1; j <= $maxgen; j++ ))      ### denotes the generations ###
			     do
				for(( m = 0; m < 1; m++))  ### denotes the no. of GA runs ###
			     	do			
					#num=$(expr $j*20| bc)
					#echo $j $num
					#head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | awk '{print $7}'|sort | head -1
					#val=$(head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | awk '{print $7}'|sort | head -1)
					#maxval=$(expr $n*$val| bc)
					#echo $maxval
					#avgiter=$(head -$num ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | grep $maxval | awk '{print $6}' | awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1)
					#echo $avgiter
					#echo "$j $m $val  $avgiter" >> ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.sorted.log
					#rm -r ./$1/${samples[$i]}/output/sorted.log
					num1=$(expr $j*20| bc)
					num2=$(expr \($j-1\)*20+1| bc)
					val=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | tail -$((($num1-$num2)+1)) | awk '{print $7}'|sort | head -1)
					maxval=$(expr $n*$val| bc)
					avgiter=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log | tail -$((($num1-$num2)+1)) | grep $maxval | awk '{print $6}' | awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1)
					echo "$j $m $val  $avgiter" >> ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.sorted.log
			     	done		
				num1=$(expr $j*25| bc)
				num2=$(expr \($j-1\)*25+1| bc)		
				#echo $num1 $num2
				avgll=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.sorted.log | tail -$((($num1-$num2)+1)) | awk '{print $3}'| awk '{sum += $1;total += 1;printf"%.3f\n", sum/total}'|tail -1)
				avgit=$(head -$num1 ./${samples[$i]}/${dataset[$p]}/${pm[$k]}.sorted.log | tail -$((($num1-$num2)+1)) | awk '{print $4}'| awk '{sum += $1;total += 1;printf"\t%d\n", sum/total}'|tail -1 )
				echo "$avgll  $avgit" >> ./${samples[$i]}/${pm[$k]}.sorted.log	
			     done
				
		done
	done
done
paste ./${samples[0]}/${pm[0]}.sorted.log ./${samples[0]}/${pm[1]}.sorted.log >>deterministic_output.log
#./${samples[0]}/${pm[2]}.sorted.log >>deterministic_dist_pc_output.log
rm -r ./${samples[0]}/${pm[0]}.sorted.log ./${samples[0]}/${pm[1]}.sorted.log #./${samples[0]}/${pm[2]}.sorted.log 
