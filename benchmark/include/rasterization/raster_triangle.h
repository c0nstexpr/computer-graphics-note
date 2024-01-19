#pragma once

#include "benchmark.h"
#include "graphics/rasterization/raster_triangle.h"

namespace graphics::benchmark
{
    auto raster_triangle(std::invocable<ankerl::nanobench::Bench&> auto bench_fn = star::empty)
    {
        using namespace std;
        using namespace stdsharp;
        using namespace glm;
        using namespace graphics::rasterization;
        using namespace ankerl::nanobench;

        auto p_value = single(i64vec2{});
        auto b_value = single(f64vec2{});
        const auto& p_out = cycle(p_value);
        const auto& b_out = cycle(b_value);
        Bench b;
        auto fn = [p_it = p_out.begin(), b_it = b_out.begin()](const auto fn) mutable
        {
            constexpr auto size = 100;
            constexpr i64vec2 p0{};
            constexpr i64vec2 p1{size, -size};

            for(auto i : iota(0, size)) fn(p0, p1, {i, size}, p_it, b_it);
        };

        b.title("raster triangle").relative(true);

        bench_fn(b);

        bench_run(b, "trivial", fn, trivial_raster_triangle<f64>);
        bench_run(b, "floating incremental", fn, floating_incremental_raster_triangle<f64>);
        bench_run(b, "integral incremental", fn, integral_incremental_raster_triangle<f64>);

        doNotOptimizeAway(*p_value.begin());
        doNotOptimizeAway(*b_value.begin());

        return b;
    }
}