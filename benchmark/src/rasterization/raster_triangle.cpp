#include "benchmark.h"
#include "random.h"
#include "triangle.h"
#include "graphics/rasterization/raster_triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace graphics::rasterization;
using namespace graphics::benchmark;
using namespace benchmark;

namespace
{
    constexpr auto iterations_count = 50000;

    void triangle_raster_benchmark(State& state, const auto& fn)
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
}

BENCHMARK_CAPTURE(triangle_raster_benchmark, noop_raster_triangle, empty_invoke)->BENCHMARK_SETUP;

BENCHMARK_CAPTURE(triangle_raster_benchmark, trivial_raster_triangle, trivial_raster_triangle<>)
    ->BENCHMARK_SETUP;

BENCHMARK_CAPTURE(triangle_raster_benchmark, floating_incremental_raster_triangle, floating_incremental_raster_triangle<>)
    ->BENCHMARK_SETUP;

BENCHMARK_CAPTURE(triangle_raster_benchmark, integral_incremental_raster_triangle, integral_incremental_raster_triangle<>)
    ->BENCHMARK_SETUP;