#include "benchmark.h"
#include "random.h"
#include "triangle.h"
#include <benchmark/benchmark.h>
#include "graphics/rasterization/raster_triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

namespace
{
    void triangle_raster_test(benchmark::State& state, const auto& fn)
    {
        std::vector<ivec2> out;
        std::vector<vec2> out_b;
        const auto max_iterations = state.max_iterations;
        const auto& data = graphics::benchmark::get_random_data<Catch::random_triangle_generator>(
            max_iterations,
            -1000,
            1000
        );

        out.reserve(max_iterations);
        out_b.reserve(max_iterations);

        for(auto _ : state)
        {
            const auto [p0, p1, p2] = data[state.iterations()];

            out.clear();
            out_b.clear();

            fn(p0, p1, p2, back_inserter(out), back_inserter(out_b));

            benchmark::DoNotOptimize(out);
            benchmark::DoNotOptimize(out_b);
            benchmark::ClobberMemory();
        }
    };
}

BENCHMARK_CAPTURE(triangle_raster_test, noop_raster_triangle, [](auto&&...) {});
BENCHMARK_CAPTURE(triangle_raster_test, trivial_raster_triangle, trivial_raster_triangle<>);
BENCHMARK_CAPTURE(triangle_raster_test, floating_incremental_raster_triangle, floating_incremental_raster_triangle<>);
BENCHMARK_CAPTURE(triangle_raster_test, integral_incremental_raster_triangle, integral_incremental_raster_triangle<>);