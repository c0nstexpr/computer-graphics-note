#pragma once

#include <ranges>

#include <stdsharp/cassert/cassert.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    inline constexpr struct trivial_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range InterpolateIn,
            typename OutIt>
        constexpr auto operator()(
            const glm::vec<L, T, Q> s0,
            decltype(s0) s1,
            const glm::mat<L + 1, L + 1, T, Q> trans,
            InterpolateIn&& interpolate_in,
            OutIt out_it
        ) const
            requires requires(std::remove_const_t<std::iter_value_t<InterpolateIn>> out_v) {
                requires star::explicitly_convertible<T, decltype(out_v)>;
                requires std::output_iterator<OutIt, decltype(out_v)>;
                requires star::arithmetic_like<decltype(out_v)>;
            }
        {
            using vec_t = glm::vec<L, T, Q>;
            using out_t = std::remove_const_t<std::iter_value_t<InterpolateIn>>;

            Expects(s0 != s1);

            const vec_t d0{trans * s0};
            const vec_t d1{trans * s1};

            {
                constexpr glm::vec<L, T, Q> zero_vec{0};

                Expects(d0 != d1);
                Expects(cross(s0, d0) == zero_vec);
                Expects(cross(s1, d1) == zero_vec);
            }

            for( //
                const auto k = d0.x * s1.x / static_cast<out_t>(d1.x * s0.x);
                const auto& in : cpp_forward(interpolate_in)
            )
                *out_it++ = 1 / (1 + k * (1 - in) / in);
        }
    } trivial_interpolate{};
}