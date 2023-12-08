#!/bin/bash

# assumes a C source file 
src=$1
basename=`echo $src | awk -F "." '{print $1}'` 

if [ -f result.csv ]; then
  rm results.csv
fi

# save the original 
cp $src $src.orig

input_size=1000
reps=1000

unroll_factors="1 2 4 8 10  20 50 100 200 400 800 1000"
for uf in ${unroll_factors}; do
	sed "s/UNROLL 1/UNROLL $uf"/ $src.orig > tmp
	mv tmp $src

	# Rpass flag to check if loop is being unrolled
	# prevent clang from vectorizing loop
	clang -O1 -Rpass=unroll -mllvm -vectorize-loops=0 -o ${basename} $src 2> /dev/null

	# if we want to do perf ...
	perf_stat_output=$(perf stat -o perf_output.txt -x, ./${basename} ${input_size} ${reps} 2>&1)

	result=$(echo "${perf_stat_output}" | grep "Result" | awk '{print $3}')
	time=$(echo "${perf_stat_output}" | grep "Scale Loop Time" | awk '{print $5}')
	counters=$(awk -F, 'NR>=2 && NR<=10 || NR==11 || NR==12 || NR==13 { printf "%s,", $1 }' perf_output.txt | sed 's/,$/\n/')

	second_time_elapsed=$(echo "${perf_stat_output}" | grep "second time elapsed" | awk '{print $1}')
	# extract exection time of target loop from built-in timer 
	#	time=`./${basename} ${input_size} ${reps} 2>&1 | grep "Loop" | awk '{print $5}'`;

	echo -e "${uf},${result},${time},${counters},${second_time_elapsed}" >> results.csv
done 

# restore orginal 
mv $src.orig $src
