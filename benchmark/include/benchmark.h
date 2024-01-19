#pragma once

#include <ranges>

#include <range/v3/view/cycle.hpp>
#include <nanobench.h>
#include <stdsharp/functional/bind_lvalue.h>

#include "graphics/namespace_alias.h"

namespace graphics::benchmark
{
    using ::ranges::views::cycle;
    using std::views::iota;
    using std::views::single;

    inline constexpr auto bench_run = []<typename... Args, std::invocable<Args...> Fn>(
                                          ankerl::nanobench::Bench& b,
                                          const std::string_view name,
                                          Fn&& fn,
                                          Args&&... args
                                      )
    {
        return b.run(name.data(), star::bind_lvalue(cpp_forward(fn), cpp_forward(args)...)); //
    };
}