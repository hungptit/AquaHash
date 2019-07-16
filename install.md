# Steps #

**Download the source code**

``` shell
git clone -r https://github.com/hungptit/AquaHash.git
```

**Compile all required libraries**

``` shell
cd AquaHash
./build.sh
```

**Compile and run all unit tests**

``` shell
cd AquaHash
cd unittests
cmake ./
make -j5
make test
```

**Run individual benchmarks**

``` shell
cd benchmark
LEN=64 ./random_string
```

Below is the sample output collected in MacOS

``` shell
LEN=64 ./random_string
String length: 64
2019-07-16 14:21:03
Running ./random_string
Run on (8 X 2200 MHz CPU s)
CPU Caches:
  L1 Data 32K (x4)
  L1 Instruction 32K (x4)
  L2 Unified 262K (x4)
  L3 Unified 6291K (x1)
Load Average: 2.08, 2.56, 2.58
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
std_hash_string               6.33 ns         6.33 ns    107056556
boost_hash_string              127 ns          106 ns      6452445
xxhash_string                 14.3 ns         14.3 ns     49247914
farmhash_string               8.79 ns         8.78 ns     78741043
clhash_string                 6.72 ns         6.71 ns    102764361
aquahash_string               5.75 ns         5.75 ns    118511496
aquahash_convert_string       5.71 ns         5.71 ns    117966262
```

**Collect performance benchmark results for different string sizes**

``` shell
./run_benchmark.sh 128
```
