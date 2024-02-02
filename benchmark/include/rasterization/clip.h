#pragma once

#include "../benchmark.h"

#include <graphics/rasterization/clip.h>

namespace graphics::benchmark
{
    inline constexpr auto clip =
        [](std::invocable<ankerl::nanobench::Bench&> auto bench_fn = star::empty)
    {
        using namespace std;
        using namespace stdsharp;
        using namespace glm;
        using namespace graphics::rasterization;
        using namespace ankerl::nanobench;

        pair<f64vec2, f64> value;
        Bench b;
        constexpr f64vec3 plane_normal{1, 1, 1};
        constexpr f64vec3 plane_p{50, 50, 50};
        const auto plane_constant = -dot(plane_normal, plane_p);

        auto fn = [&](const auto fn, const auto& arg)
        {
            constexpr auto size = 100;

            for(auto i = 0; i < size; ++i)
                for(auto j = 0; j < size; ++j)
                    value = fn( //
                        plane_normal,
                        arg,
                        f64mat2x3{f64vec3{100, 100, 100}, f64vec3{i, j, 100}} //
                    );
        };

        b.title("line clip").relative(true);

        bench_fn(b);

        bench_run(b, "by plane point", fn, line_clip, plane_p);
        bench_run(b, "by plane constant", fn, line_clip, plane_constant);

        doNotOptimizeAway(value);

        return b;
    };
}