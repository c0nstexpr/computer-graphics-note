#include "rasterization/raster_line.h"
#include "graphics/rasterization/raster_line.h"
#include "test.h"

using namespace std;
using namespace ::ranges;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace graphics::rasterization;
using namespace ankerl::nanobench;

namespace graphics::benchmark
{
    static constexpr auto min = -1000;
    static constexpr auto max = 1000;
    static constexpr size_t value_range = max - min;

    ankerl::nanobench::Bench raster_line(const unsigned iterations, const seed_t seed)
    {
        std::mt19937_64 engine{seed};

        const auto& random_points = get_random_data(
            iterations,
            random_glm_vec_generator<4, int64, random_engine_wrapper<>>{min, max, engine}
        );
        const auto& random_d = get_random_data(
            iterations,
            random_glm_vec_generator<2, int64, random_engine_wrapper<>>{min, max, engine}
        );
        const cycled_view p_rng{random_points};
        const cycled_view d_rng{random_d};
        vector<i64vec2> out{value_range};
        auto fn = [&, p_it = p_rng.begin(), d_it = d_rng.begin()](const auto fn) mutable
        {
            const auto& p = *p_it++;

            fn(vec<2, int64>{p.x, p.y}, {p.z, p.w}, out.begin(), *d_it++);

            doNotOptimizeAway(out);
        };
        Bench b;

        b.title("draw line").warmup(3).minEpochIterations(iterations).relative(true);

        bench_run(b, "trivial", fn, trivial_raster_line);
        bench_run(b, "bresenham incremental", fn, bresenham_raster_line);

        return b;
    }
}