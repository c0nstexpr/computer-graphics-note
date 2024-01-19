#pragma once

#include <ranges>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    template<typename OutT = float>
    struct trivial_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range InterpolateIn>
        constexpr auto operator()(
            const glm::vec<L, T, Q> s0,
            decltype(s0) s1,
            std::remove_const_t<decltype(s0)> d0,
            decltype(s0) d1,
            InterpolateIn&& interpolate_in,
            std::output_iterator<OutT> auto out_it
        ) const
            requires std::
                constructible_from<glm::vec<L, OutT, Q>, std::iter_reference_t<InterpolateIn>>
        {
            using vec_t = glm::vec<L, T, Q>;
            using out_vec_t = glm::vec<L, T, Q>;

            for(const auto& i : std::views::transform(
                    cpp_forward(interpolate_in),
                    [](auto& v) { return out_vec_t{v}; }
                ))
            {
                out_vec_t out{};
                for(glm::length_t i = 0; i < L; ++i)
                    out[i] = ;

                *out_it++ = out;
            }
        }
    };
}