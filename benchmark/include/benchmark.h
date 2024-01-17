#pragma once

#include <nanobench.h>
#include <range/v3/view/cycle.hpp>
#include <stdsharp/functional/invoke.h>

#include "random.h"
#include "test.h"

inline constexpr auto bench_run = []<typename... Args, std::invocable<Args...> Fn>(
                                      ankerl::nanobench::Bench& b,
                                      const std::string_view name,
                                      Fn&& fn,
                                      Args&&... args
                                  )
{
    return b.run(name.data(), std::bind_front(cpp_forward(fn), cpp_forward(args)...)); //
};
