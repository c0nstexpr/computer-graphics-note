#include <benchmark/benchmark.h>
#include <stdsharp/functional/invoke.h>

#include "test.h"

using stdsharp::empty_invoke;

#define BENCHMARK_SETUP          \
    Iterations(iterations_count) \
        ->MinWarmUpTime(0.1)     \
        ->MinTime(2)             \
        ->Repetitions(3)
