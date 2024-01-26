#pragma once

#include "raster_triangle.h"

namespace graphics::rasterization
{
    inline constexpr struct trivial_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range InCoord,
            typename OutIt>
        constexpr auto operator()(
            const std::array<glm::vec<3, T, Q>, 3> src_triangle,
            decltype(src_triangle) dst_triangle,
            InCoord&& coord,
            OutIt out_it
        ) const
            requires requires(
                std::remove_const_t<std::iter_value_t<InCoord>> coord,
                decltype(coord)::out_vec_t::value_type out_t,
                barycentric_coordinate<3, decltype(out_t), T> barycentric
            ) {
                requires std::same_as<decltype(coord), decltype(barycentric)>;
                requires std::output_iterator<OutIt, decltype(barycentric)>;
            }
        {
            using vec_t = glm::vec<L, T, Q>;
            using out_t = std::remove_const_t<std::iter_value_t<InCoord>>;


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