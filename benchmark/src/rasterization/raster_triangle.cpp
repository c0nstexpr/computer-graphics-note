#include <range/v3/view/cycle.hpp>

#include "rasterization/raster_triangle.h"
#include "graphics/rasterization/raster_triangle.h"
#include "random.h"
#include "triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace graphics::rasterization;
using namespace graphics::benchmark;
using namespace ankerl::nanobench;

Bench raster_triangle_benchmark(const seed_t seed)
{
    static constexpr auto iterations = 20000;
    static constexpr auto min = -100;
    static constexpr auto max = 100;
    static constexpr size_t value_range = max - min;

    const auto& random_triangles =
        get_random_data(iterations, Catch::random_triangle_generator<int64>{min, max, seed});
    std::vector<i64vec2> out{value_range * value_range};
    std::vector<f64vec2> out_b{out.size()};
    const auto& rng = ::ranges::views::cycle(random_triangles);
    auto fn = [&, it = rng.begin()](const auto fn) mutable
    {
        const auto& [p0, p1, p2] = *(it++);

        fn(p0, p1, p2, out.begin(), out_b.begin());

        doNotOptimizeAway(out);
        doNotOptimizeAway(out_b);
    };
    Bench b;

    b.title("raster triangle").warmup(3).minEpochIterations(iterations).relative(true);

    bench_run(b, "trivial", fn, trivial_raster_triangle<f64>);
    bench_run(b, "floating incremental", fn, floating_incremental_raster_triangle<f64>);
    bench_run(b, "integral incremental", fn, integral_incremental_raster_triangle<f64>);

    return b;
}