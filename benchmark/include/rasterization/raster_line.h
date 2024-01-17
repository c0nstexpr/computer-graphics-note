#pragma once
#include "benchmark.h"

namespace graphics::benchmark
{
    ankerl::nanobench::Bench raster_line(unsigned iterations, seed_t seed);
}