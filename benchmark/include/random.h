#pragma once

#include <algorithm>
#include <random>

#include <catch2/generators/catch_generators.hpp>

#include <stdsharp/macros.h>

namespace graphics::benchmark
{
    using seed_t = std::mt19937_64::result_type;

    inline constexpr struct
    {
        template<typename Gen>
            requires std::derived_from<Gen, Catch::Generators::IGenerator<typename Gen::type>>
        constexpr auto operator()(const std::size_t n, Gen gen) const
        {
            std::vector<typename Gen::type> a{n};
            std::ranges::generate(
                a,
                [&gen] mutable
                {
                    auto&& t = gen.get();
                    gen.next();
                    return cpp_move(t);
                }
            );
            return a;
        }
    } get_random_data{};
}
