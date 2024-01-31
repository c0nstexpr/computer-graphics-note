#pragma once

#include "../benchmark.h"
#include "graphics/rasterization/raster_triangle.h"
#include <glm/fwd.hpp>

namespace graphics::benchmark
{
    inline constexpr auto raster_triangle = [](std::invocable<ankerl::nanobench::Bench&> auto bench_fn = star::empty)
    {
        using namespace std;
        using namespace stdsharp;
        using namespace glm;
        using namespace graphics::rasterization;
        using namespace ankerl::nanobench;

        constexpr auto size = 50;
        constexpr f64vec2 window_max{size / static_cast<f64>(2)};
        constexpr i64vec2 p0{};
        constexpr i64vec2 p1{size, -size};

        auto value = single(barycentric_coordinate<2, f64, i64>{});
        const auto& out = cycle(value);
        Bench b;

        auto fn =
            [size,
             p0,
             p1,
             it = out.begin(),
             predicate =
                 raster_triangle_windowed_predicate<i64>{-window_max, window_max}](const auto fn)
        {
            for(auto i : iota(0, size)) fn(array{p0, p1, i64vec2{i, size}}, it, predicate);
        };

        b.title("raster triangle").relative(true);

        bench_fn(b);

        bench_run(b, "trivial", fn, trivial_raster_triangle<f64>);
        bench_run(b, "integral incremental", fn, integral_incremental_raster_triangle<f64>);

        doNotOptimizeAway(*out.begin());

        return b;
    };
}