#pragma once
#include "random.h"
#include "benchmark.h"
#include "triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace graphics::rasterization;
using namespace graphics::benchmark;

void triangle_raster_benchmark(const auto& fn)
{
    static constexpr auto min = -100;
    static constexpr auto max = 100;
    static constexpr size_t value_range = max - min;
    static const auto& data =
        get_random_data<random_triangle_generator<int64>>(iterations_count, min, max);

    static vector<i64vec2> out{value_range * value_range};
    static vector<f64vec2> out_b{out.size()};

    for(auto _ : state)
    {
        const auto& [p0, p1, p2] = data[state.iterations()];

        fn(p0, p1, p2, out.begin(), out_b.begin());

        DoNotOptimize(out);
        DoNotOptimize(out_b);
        ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations());
};