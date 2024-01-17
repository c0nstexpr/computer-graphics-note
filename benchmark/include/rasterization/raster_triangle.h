#pragma once
#include "benchmark.h"

namespace graphics::benchmark
{
    ankerl::nanobench::Bench raster_triangle(unsigned iterations, seed_t seed);
}