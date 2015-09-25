#
for ((i = 50;i <= 95;i += 5))
do 
echo "awk 'NR!=1&&\$1!=t{print >\"final_ws_$i.dat\"}{t=\$1}' final_working_set.dat-$i"
#awk "'NR!=1&&\$1!=t{print >\"final_ws_$i.dat\"}{t=\$1}' final_working_set.dat-$i"
done