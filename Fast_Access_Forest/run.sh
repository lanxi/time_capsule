#!/bin/bash
#./cal_ground_truth
#./working_set_calculator
for ((i = 50;i <= 95;i +=5))
do
	./generate_working_set_files $i "working_set.dat-$i"
done