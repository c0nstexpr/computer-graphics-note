#pragma once

#include <glm/ext/matrix_clip_space.hpp>

#include "../benchmark.h"
#include "graphics/rasterization/perspective_interpolate.h"

namespace graphics::benchmark
{
    inline constexpr auto perspective_interpolate =
        [](std::invocable<ankerl::nanobench::Bench&> auto bench_fn = star::empty)
    {
        using namespace std;
        using namespace stdsharp;
        using namespace glm;
        using namespace graphics::rasterization;
        using namespace ankerl::nanobench;

        const auto mat = perspective<f64>(radians(90.), 1., 1., 100.);
        const auto projected_triangle =
            mat * f64mat3x4{{30, -30, -40, 1}, {-30, -30, -50, 1}, {0, 30, -60, 1}};
        const f64vec3 depths{
            projected_triangle[0].w,
            projected_triangle[1].w,
            projected_triangle[2].w
        };

        constexpr auto size = 10000;
        array<f64vec3, size> barycentric{};
        auto value = single(f64vec3{});
        const auto& out = cycle(value);
        const auto it = out.begin();

        Bench b;

        const auto fn = [&](const auto& fn, auto&&... args)
        {
            fn(cpp_forward(args)..., barycentric, it); //
        };

        for(constexpr auto a = .5; auto i : iota(0, size))
        {
            const auto b = static_cast<f64>(i) / size * (1 - a);
            barycentric[i] = f64vec3{a, b, 1 - a - b};
        }

        b.title("perspective interpolate").relative(true);

        bench_fn(b);

        bench_run(b, "trivial", fn, trivial_perspective_interpolate, projected_triangle, mat);
        bench_run(b, "depth multiply", fn, depth_mul_perspective_interpolate, depths);
        bench_run(b, "depth division", fn, depth_div_perspective_interpolate, depths);

        doNotOptimizeAway(*value.begin());

        return b;
    };
}