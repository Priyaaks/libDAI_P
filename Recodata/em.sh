#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=out.log
#arg[3]=em file
#arg[4]=number of factors
#
#declare samples=(100 250 500 1000 2000 4000)
declare samples=(500)
 num=11
echo $num
for (( p = 0; p < 1; p++ ))
do
	for (( k = 0; k < 1; k++ ))      ### denotes the dataset ###
	do
		for (( j = 1 ; j < $num; j++ )) ### denotes the data file ###
		do
		echo "factor graph file is" +/home/priya/libDAI-0.3.1_old/Movie5Node1_factor_graphs/Movie4NodeBN1_$j.fg
		
		   #echo "data file"+/home/priya/libDAI-0.3.1/$1/${samples[$p]}/$j.tab
		  #/home/priya/libDAI-0.3.1/examples/example_Newasia_em /home/priya/libDAI-0.3.1/$1/${samples[$p]}/$j.tab /home/priya/libDAI-0.3.1/$1/${samples[$j]}/$2 $3 /home/priya/libDAI-0.3.1/$1_factor_graphs/$1_$k.fg
		  /home/priya/libDAI-0.3.1_old/examples/example_Newasia_em /home/priya/libDAI-0.3.1_old/Movie4NodeBN1/u1base200.tab out /home/priya/libDAI-0.3.1_old/4NodeMovieBN3.em /home/priya/libDAI-0.3.1_old/Movie4Node3_factor_graphs/Movie4NodeBN1_$j.fg out1_4NodeBN3

		done
	done
done
