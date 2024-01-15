#include <range/v3/view/cycle.hpp>

#include "rasterization/raster_triangle.h"
#include "graphics/rasterization/raster_triangle.h"
#include "random.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace graphics::rasterization;
using namespace graphics::benchmark;
using namespace ankerl::nanobench;

namespace
{
    template<auto Fn>
    static constexpr auto raster_triangle_benchmark_impl = [](const seed_t seed)
    {
        const auto& [p0, p1, p2] =
            *get_random_data(iterations, random_triangle_generator<int64>{min, max, seed});

        Fn(p0, p1, p2, out.begin(), out_b.begin());

        doNotOptimizeAway(out);
        doNotOptimizeAway(out_b);
    };
}

Bench& raster_triangle_benchmark_fn::operator()(Bench& b, const seed_t seed)
{
    b.title("raster triangle")
        .warmup(3)
        .minEpochIterations(iterations)
        .relative(true);

        bench_run("trivial", raster_triangle_benchmark_impl<trivial_raster_triangle<f64>>, seed);

        .run("floating incremental", raster_triangle_benchmark_impl<floating_incremental_raster_triangle<f64>>)
        .run("integral incremental", raster_triangle_benchmark_impl<integral_incremental_raster_triangle<f64>>);
}