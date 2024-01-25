#pragma once

#include "../benchmark.h"
#include "graphics/rasterization/raster_line.h"

namespace graphics::benchmark
{
    auto raster_line(std::invocable<ankerl::nanobench::Bench&> auto bench_fn = star::empty)
    {
        using namespace std;
        using namespace stdsharp;
        using namespace glm;
        using namespace graphics::rasterization;
        using namespace ankerl::nanobench;

        auto value = single(i64vec2{});
        const auto& out = cycle(value);
        Bench b;
        auto fn = [it = out.begin()](const auto fn) mutable
        {
            constexpr auto size = 1000;
            for(auto i : iota(0, size))
                for(auto j : iota(0, size)) fn(i64vec2{}, {size, i}, it, {j, j});
        };

        b.title("raster line").relative(true);

        bench_fn(b);

        bench_run(b, "trivial", fn, trivial_raster_line);
        bench_run(b, "bresenham", fn, bresenham_raster_line);

        doNotOptimizeAway(*value.begin());

        return b;
    }
}