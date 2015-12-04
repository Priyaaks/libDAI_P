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
		for (( k = 0; k < 4; k++ ))      ### denotes the mutation probability ###
		  do  
				printf "${pm[$k]} mutation probability \t\n " >> ./${samples[$i]}/${pm[$k]}.time.log
			  	for(( m = 0; m < 25; m++))  ### denotes the no. of GA runs ###
			     	do			
					tail  -1 ./${samples[$i]}/${dataset[$p]}/$m/${pm[$k]}/${pm[$k]}.log >> ./${samples[$i]}/${pm[$k]}.time.log
			     	done
		done
	done
done
paste ./${samples[0]}/${pm[0]}.time.log ./${samples[0]}/${pm[1]}.time.log ./${samples[0]}/${pm[2]}.time.log ./${samples[0]}/${pm[3]}.time.log >>time_baseline.log
rm -r ./${samples[0]}/${pm[0]}.time.log ./${samples[0]}/${pm[1]}.time.log ./${samples[0]}/${pm[2]}.time.log ./${samples[0]}/${pm[3]}.time.log
