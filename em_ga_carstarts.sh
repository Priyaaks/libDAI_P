#!/bin/bash
#
#arg[1]=datafolder (same as factor graph file)
#
#declare samples=(100 250 500 1000 2000 4000)

declare samples=(500)
#declare tabfile=(3 4 7 16)
declare tabfile=(7)
declare pm=(0.05555 0.3 0.5)
#declare fgfile=(0 499 500 999 1000 1499 1500 1999 2000 2499 2500 2999 3000 3499 3500 3999 4000 4499 4500 4999)
#5000 5499 5500 5999 6000 6499 6500 6999 7000 7499 7500 7999 8000 8499 8500 8999 9000 9499 9500 9999 10000 10499 11500 11999 12000 12499)
tLen=${#pm[@]}
tLen1=${#tabfile[@]}
tLen2=${#samples[@]}
declare pc=(0.1 0.2 0.5 0.8)
#pc = 0.1
for (( q = 1; q < 25; q++ )) ### GA runs###
do
	for (( p = 0; p < 1; p++ )) ### samples ###
	do
	 mkdir /home/priya/libDAI-0.3.1/${samples[$p]}
		for (( j = 0 ; j < 1; j++ )) ### denotes the data file ###
		do
		 mkdir -p /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q
			for (( k = 0 ; k <3; k++ )) ### denotes the mutation probability ###
			do
			   mkdir -p /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
			   mkdir /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/learned
			   #l=${fgfile[$k]} 
			   #u=${fgfile[$k+1]}
			   cp /home/priya/libDAI-0.3.1/carstarts_factor_graphs/carstarts_*.fg /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
			   #cp /home/priya/libDAI-0.3.1/carstarts_fg/carstarts_{4500..4999}.fg /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
			   #echo "data file" /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab
			   #echo "out log" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log
			   #echo "pm" ${pm[$k]}
			   #echo "pc" ${pc[0]}
			   #echo "alarmlearnedfgloc" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/
			   /home/priya/libDAI-0.3.1/examples/example_gaem_deterministic /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log ${pm[0]} ${pc[0]} /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/
			  #/home/priya/libDAI-0.3.1/examples/example_Newasia_mtem /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log $q ${pc[0]} /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0/
##>> /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/out.log
			done
		done
	done
done
