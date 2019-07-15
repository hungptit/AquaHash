#!/bin/bash
N=1024
mkdir -p data
osType=$(uname)

# Compile all benchmarks
cmake ./
make -j5

# Run all benchmark and plot the collected data.
data_dir="data/$osType/string/"
mkdir -p $data_dir
for idx in `seq 1 1 $N`;
do
    LEN=$idx ./random_string --benchmark_format=json --benchmark_out="$data_dir/data_$idx.json"
done
./plot_benchmark_results.py data/data_ $N