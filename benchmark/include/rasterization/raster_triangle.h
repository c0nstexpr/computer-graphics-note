#pragma once
#include "benchmark.h"
#include "triangle.h"

struct raster_triangle_benchmark_fn
{
    static constexpr auto iterations = 20000;
    static constexpr auto min = -100;
    static constexpr auto max = 100;
    static constexpr size_t value_range = max - min;

    std::vector<graphics::test::triangle<glm::i64>> random_triangles;
    std::vector<glm::i64vec2> out{value_range * value_range};
    std::vector<glm::f64vec2> out_b{out.size()};

    ankerl::nanobench::Bench& operator()(ankerl::nanobench::Bench& b, seed_t seed);
};