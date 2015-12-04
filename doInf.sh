#mkdir "new"

#Using the learned factor graphs on testing data
#echo "error_instances success_instances error_percent rmse tp tn fp fn" > out.log
#------5nodeBN--------
for (( q = 1; q < 11; q++ )) ### GA runs###
do

./examples/doInference ./Recodata/5NodeMovieBN1/u1test200.tab out1.log ./Recodata/5NodeMovieBN1.em ./Recodata/5NodeMovieBN1_factor_graphs/Movie4NodeBN1_L$q.fg
#./examples/doInference ./Recodata/400/Movie5NodeBN1/u1test200.tab out1.log ./Recodata/400/5NodeMovieBN1.em ./Recodata/400/Movie5Node1_factor_graphs/Movie5NodeBN1_L$q.fg
done
#------4nodeBN3--------
#for (( q = 1; q < 6; q++ )) ### GA runs###
#do
#./examples/doInference ./Recodata/400/Movie4NodeBN1/u1test200.tab out1.log ./Recodata/400/4NodeMovieBN3.em ./Recodata/400/Movie4Node3_factor_graphs/Movie4NodeBN3_L$q.fg
#done
#------4nodeBN2--------
#for (( q = 1; q < 11; q++ )) ### GA runs###
#do
#./examples/doInference ./Recodata/4NodeMovieBN2/u1test200.tab out1.log ./Recodata/400/4NodeMovieBN2.em ./Recodata/4NodeMovieBN2_factor_graphs/4NodeMovieBN2_L$q.fg
#done
#------4nodeBN1--------
#for (( q = 1; q < 11; q++ )) ### GA runs###
#do
#./examples/doInference ./Recodata/Movie4NodeBN1/u1base200.tab out.log ./Recodata/4NodeMovieBN1.em ./Recodata/Movie4Node1_factor_graphs/Movie4NodeBN1_L$q.fg
#done

#~~~~~~~~~ To Run EM on training data
#------5nodeBN1--------
#for (( q = 3; q < 5; q++ )) ### GA runs###
#do
#./examples/example_Newasia_em ./Recodata/Movie5NodeBN1/u1base400.tab out.log ./Recodata/400/5NodeMovieBN1.em ./Recodata/400/Movie5Node1_factor_graphs/Movie5NodeBN1_$q.fg ./Recodata/400/Movie5Node1_factor_graphs/Movie5NodeBN1_L$q.fg
#done
#------4nodeBN3--------
#for (( q = 2; q < 6; q++ )) ### GA runs###
#do
# ./examples/example_Newasia_em ./Recodata/400/Movie4NodeBN1/u1base200.tab out.log ./Recodata/400/4NodeMovieBN3.em ./Recodata/400/Movie4Node3_factor_graphs/Movie4NodeBN3_$q.fg ./Recodata/400/Movie4Node3_factor_graphs/Movie4NodeBN3_L$q.fg
#done

#------4nodeBN2--------
#for (( q = 1; q < 6; q++ )) ### GA runs###
#do
# ./examples/example_Newasia_em ./Recodata/400/Movie4NodeBN1/u1base200.tab out.log ./Recodata/400/4NodeMovieBN2.em ./Recodata/400/Movie4Node2_factor_graphs/Movie4NodeBN2_$q.fg ./Recodata/400/Movie4Node2_factor_graphs/Movie4NodeBN2_L$q.fg
#done

#------4nodeBN1--------
#for (( q = 1; q < 6; q++ )) ### GA runs###
#do
#./examples/example_Newasia_em ./Recodata/400/Movie4NodeBN1/u1base200.tab out.log ./Recodata/400/4NodeMovieBN1.em ./Recodata/400/Movie4Node1_factor_graphs/Movie4NodeBN1_$q.fg ./Recodata/400/Movie4Node1_factor_graphs/Movie4NodeBN1_L$q.fg
#done