#!/usr/bin/env bash

[ ! -d "FlameGraph" ] && git clone --depth=1 https://github.com/brendangregg/FlameGraph

cc -o bench uu.c uu.h ubench.h bench.c -Wall -Wextra -Werror -std=gnu99 -O3 -lm

perf record -F 997 -g ./bench
perf script -i perf.data 1> perf.unfold

FlameGraph/stackcollapse-perf.pl perf.unfold 1> perf.folded
FlameGraph/flamegraph.pl perf.folded 1> perf.svg

rm -f perf.data perf.data.old perf.unfold perf.folded