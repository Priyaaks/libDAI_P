#mkdir "new"
for (( q = 0; q < 10; q++ )) ### GA runs###
do 
 #grep "Alarm_"$q analysis > ./new/alarm_$q.log
 #cat ./libDAI-0.3.1/0.log | awk -v p="$q" '$2 == p'> ./new/3NodeBN13_$q.log
 #echo $q
 cat /Applications/Utilities/libDAI-0.3.1/500/19/0/0.3/0.3.log | awk -v p="$q" '$5 == p'> /Applications/Utilities/libDAI-0.3.1/500/19/0/0.3/alarm_$q.log
done
