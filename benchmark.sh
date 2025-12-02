#!/bin/bash
# parts of this file were written using Generative AI. I was struggling to figure out how to port my shell script from Linux to macOS. 

CXX=/opt/homebrew/opt/llvm/bin/clang++
RUNS=3
SIZE=20000000
THREADS=(1 2 4 8)
ALGOS=("merge" "quick" "radix")

echo "Compiling with Homebrew LLVM..."
for a in "${ALGOS[@]}"; do
    $CXX -std=c++17 -O3 -fopenmp "$a.cpp"   -o "$a"
    $CXX -std=c++17 -O3 -fopenmp "${a}p.cpp" -o "${a}p"
done
echo "Compilation OK"
echo

time_run() {
    exe=$1
    threads=$2
    total=0

    for ((i=1; i<=RUNS; i++)); do
        if [[ $threads -eq 0 ]]; then
            start=$(date +%s%N)
            ./"$exe" $SIZE >/dev/null
            end=$(date +%s%N)
        else
            start=$(date +%s%N)
            OMP_NUM_THREADS=$threads ./"$exe" $SIZE >/dev/null
            end=$(date +%s%N)
        fi
        total=$((total + end - start))
    done

    avg=$(echo "scale=6; $total / $RUNS / 1000000000" | bc -l)
    echo "$avg"
}

for algo in "${ALGOS[@]}"; do
    echo "======================================"
    echo "Algorithm: $algo (SIZE = $SIZE)"
    echo "======================================"

    serial_avg=$(time_run "$algo" 0)
    echo "Serial avg: $serial_avg sec"

    for t in "${THREADS[@]}"; do
        par_avg=$(time_run "${algo}p" $t)
        speed=$(echo "scale=2; $serial_avg / $par_avg" | bc -l)
        printf "Threads: %2d | Parallel avg: %7s sec | Speedup: %sx\n" \
            "$t" "$par_avg" "$speed"
    done
    echo
done
