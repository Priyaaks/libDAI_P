#
#arg[1]=datafolder (same as factor graph file)
#
#declare samples=(100 250 500 1000 2000 4000)

declare samples=(500)
declare tabfile=(19)
#declare tabfile=(alarm_501)
#declare tabfile=(7 13 19 28 33)
#declare tabfile=(3NodeBN1_3)
declare pm=(0.02702 0.5 0.5)
tLen=${#pm[@]}
tLen1=${#tabfile[@]}
tLen2=${#samples[@]}
declare pc=(0.1 0.2 0.5 0)
#pc = 0.1
for (( q = 0; q < 1; q++ )) ### GA runs###
do
	for (( p = 0; p < 1; p++ )) ### samples ###
	do
	 #mkdir /home/priya/libDAI-0.3.1/${samples[$p]}
		for (( j = 0 ; j < 1; j++ )) ### denotes the data file ###
		do
		 #mkdir -p /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q
			for (( k = 1 ; k <2; k++ )) ### denotes the mutation probability ###
			do
mkdir -p /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
#mkdir -p /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/0
#  mkdir /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/learned
# mkdir /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/learned
#cp /home/priya/libDAI-0.3.1/$1_factor_graphs/$1_*.fg /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
cp /Applications/Utilities/libDAI-0.3.1/$1_factor_graphs/$1_{0..19}.fg /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0
#cp /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4Node2_factor_graphs/Movie4NodeBN2_{0..4}.fg /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/0
			   #echo "data file" /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab
			   #echo "out log" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log
			   #echo "pm" ${pm[$k]}
			   #echo "pc" ${pc[0]}
			   #echo "alarmlearnedfgloc" /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/
			  #/home/priya/libDAI-0.3.1/examples/example_Newasia_mtem /home/priya/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log $q ${pc[0]} /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/0/
#>> /home/priya/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/out.log


# FOR RECO DATA
#/Applications/Utilities/libDAI-0.3.1/examples/example_gaem_deterministic /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN1/u1base200.tab /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/${pm[$k]}.log ${pm[k]} ${pc[3]} /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/ /Applications/Utilities/libDAI-0.3.1/Recodata/Movie4NodeBN2_EM1/$q/${pm[$k]}/$q.log Movie4NodeBN2 /Applications/Utilities/libDAI-0.3.1/Recodata/4NodeMovieBN2.em 4
#/Applications/Utilities/libDAI-0.3.1/examples/example_gaem_deterministic /Applications/Utilities/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/${pm[$k]}.log ${pm[k]} ${pc[3]} /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/ /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/$q.log $1 /Applications/Utilities/libDAI-0.3.1/alarm.em 36
#/Applications/Utilities/libDAI-0.3.1/examples/example_gaem_deterministic /Applications/Utilities/libDAI-0.3.1/$1/${samples[$p]}/${tabfile[$j]}.tab /Applications/Utilities/libDAI-0.3.1/$1_factor_graphs ${pm[k]} ${pc[3]} /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/$q.log $1 /Applications/Utilities/libDAI-0.3.1/$1.em
/Applications/Utilities/libDAI-0.3.1/examples/example_gaem_deterministic /Applications/Utilities/libDAI-0.3.1/Recodata/$1/u1base200.tab /Applications/Utilities/libDAI-0.3.1/Recodata/$1_factor_graphs ${pm[k]} ${pc[3]} /Applications/Utilities/libDAI-0.3.1/Recodata/$1/$q.log $1 /Applications/Utilities/libDAI-0.3.1/Recodata/$1.em
#>> /Applications/Utilities/libDAI-0.3.1/${samples[$p]}/${tabfile[$j]}/$q/${pm[$k]}/out.log
			done
		done
	done
done
