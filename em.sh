#
#arg[1]=datafolder (same as factor graph file)
#arg[2]=out.log
#arg[3]=em file
#arg[4]=number of factors
#
#declare samples=(100 250 500 1000 2000 4000)
declare samples=(500)
 num=$4
echo $num
for (( p = 0; p < 1; p++ ))
do
	for (( k = 0; k < 10; k++ ))      ### denotes the dataset ###
	do
		echo "factor graph file is" +/home/priya/libDAI-0.3.1/$1_factor_graphs/$1_$k.fg
		for (( j = 0 ; j < 1; j++ )) ### denotes the data file ###
		do
		   echo "data file"+/home/priya/libDAI-0.3.1/$1/${samples[$p]}/$num.tab
		  /home/priya/libDAI-0.3.1/examples/example_Newasia_em /home/priya/libDAI-0.3.1/$1/${samples[$p]}/$num.tab /home/priya/libDAI-0.3.1/$2 $3 /home/priya/libDAI-0.3.1/$1_factor_graphs/$1_$k.fg out1.log
 # /home/priya/libDAI-0.3.1/examples/example_Newasia_em /home/priya/libDAI-0.3.1/$1/${samples[$p]}/$num.tab /home/priya/libDAI-0.3.1/$2 $3 /home/priya/libDAI-0.3.1/500/19/0/0.3/learned/0/$1_$k.fg out1.log
		done
	done
done
