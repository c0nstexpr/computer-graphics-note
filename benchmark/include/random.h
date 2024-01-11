#pragma once

#include <algorithm>
#include <catch2/generators/catch_generators.hpp>
#include <stdsharp/macros.h>

namespace graphics::benchmark
{
    template<typename Gen>
        requires std::derived_from<Gen, Catch::Generators::IGenerator<typename Gen::type>>
    struct get_random_data_fn
    {
    private:
        using type = Gen::type;

        static constexpr auto impl(const std::size_t n, auto&&... args)
        {
            std::vector<type> a{n};
            std::ranges::generate(
                a,
                [gen = Gen{cpp_forward(args)...}] mutable
                {
                    auto&& t = gen.get();
                    gen.next();
                    return cpp_move(t);
                }
            );
            return a;
        }

    public:
        template<typename... Args>
            requires std::constructible_from<Gen, Args...>
        constexpr auto& operator()(const std::size_t n, Args&&... args) const
        {
            static const auto a = impl(n, cpp_forward(args)...);
            return a;
        }
    };

    template<typename T>
    inline constexpr get_random_data_fn<T> get_random_data{};
}