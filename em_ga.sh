#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=out.log
#arg[3]=em file
#arg[4]=number of factors
#
#declare samples=(100 250 500 1000 2000 4000)
declare samples=(500 1000)
declare tabfile=(7 13 19 28 33)
declare pm=(0.001 0.002 0.005 0.1 0.3 0.5)
tLen=${#pm[@]}
tLen1=${#tabfile[@]}
tLen2=${#samples[@]}
declare pc=(0.1 0.2 0.5 0.8 1)
#pc = 0.1
for (( p = 0; p < 1; p++ ))
do
 mkdir /home/priya/libDAI-0.3.1/${samples[$p]}
	for (( j = 0 ; j < 1; j++ )) ### denotes the data file ###
	do
	 mkdir /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}
		for (( k = 0 ; k < 1; k++ )) ### denotes the mutation probability ${tLen} ###
		do
		   mkdir -p /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/0
		   mkdir /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/learned
		   cp -v /home/priya/libDAI-0.3.1/alarm_factor_graphs/*.fg /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/0
		   echo "data file" /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab
		   echo "out log" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/${pm[$k]}.log
		   echo "pm" ${pm[$k]}
		   echo "pc" ${pc[0]}
		   echo "alarmlearnedfgloc" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/
		  /home/priya/libDAI-0.3.1/examples/example_gaem_deterministic_distance /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/${pm[$k]}.log ${pm[$k]} ${pc[1]} /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/${pm[$k]}/
		done
	done
done
