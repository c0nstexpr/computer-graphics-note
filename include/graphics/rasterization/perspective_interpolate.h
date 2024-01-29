#pragma once

#include <range/v3/numeric/accumulate.hpp>

#include "../traits.h"
#include "raster_triangle.h"

namespace graphics::rasterization
{
    inline constexpr struct perspective_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range BarycentricCoordRng,
            typename OutIt>
            requires requires(
                glm_vec_traits<std::decay_t<std::iter_value_t<BarycentricCoordRng>>> coord
            ) {
                requires decltype(coord)::length == L;
                requires decltype(coord)::qualifier == Q;
                requires std::output_iterator<OutIt, typename decltype(coord)::vec>;
            }
        constexpr auto operator()(
            const glm::vec<L, T, Q>& depth_coords,
            BarycentricCoordRng&& barycentric_coords,
            OutIt out_it
        ) const
        {
            for(glm::length_t i = 0; i < L; ++i) Expects(depth_coords[i] != 0);

            for(const auto out_depths =
                    static_cast<std::decay_t<std::iter_value_t<BarycentricCoordRng>>>(depth_coords);
                auto coord : cpp_forward(barycentric_coords))
            {
                coord /= out_depths;
                *out_it++ = coord / ::ranges::accumulate(coord);
            }
        }
    } perspective_interpolate{};
}