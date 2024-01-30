#pragma once

#include <algorithm>
#include <ranges>

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/numeric/inner_product.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/zip_with.hpp>

#include <glm/gtx/intersect.hpp>

#include "../traits.h"

namespace graphics::rasterization
{
    inline constexpr struct trivial_perspective_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range BarycentricCoordRng,
            typename OutIt>
            requires std::same_as<
                         std::decay_t<std::ranges::range_value_t<BarycentricCoordRng>>,
                         glm::vec<L - 1, T, Q>> &&
            (L > 1)
        constexpr auto operator()(
            const std::array<glm::vec<L, T, Q>, L - 1>& homo_coords,
            std::type_identity_t<glm::mat<L, L, T, Q>> trans,
            BarycentricCoordRng&& barycentric_coords,
            OutIt out_it
        ) const
        {
            using homogenous_vec = glm::vec<L, T, Q>;

            constexpr auto dimension = L - 1;
            using vec = glm::vec<dimension, T, Q>;

            trans = glm::inverse(trans);

            std::array<homogenous_vec, dimension> src_coords = homo_coords;

            for(auto& src_coord : src_coords)
            {
                src_coord = trans * src_coord;
                src_coord /= src_coord[dimension];
            }

            glm::mat<dimension, dimension, T, Q> inv;
            {
                decltype(inv) matrix;
                for(glm::length_t i = 0; i < dimension; ++i)
                    matrix[i] = src_coords[i] - src_coords[0];
                inv = glm::inverse(matrix);
            }

            for(const auto& coord : cpp_forward(barycentric_coords))
            {
                const auto src_p =
                    trans * ::ranges::inner_product(src_coords, coord, homogenous_vec{});
                *out_it++ = (src_p / src_p[dimension] - src_coords[0]) * inv;

                // if constexpr([[maybe_unused]] T d{}; L == 4)
                // {
                //     glm::vec<2, T, Q> projected_bc;

                //     glm::intersectRayTriangle(
                //         vec{},
                //         vec{src_p},
                //         vec{src_coords[0]},
                //         vec{src_coords[1]},
                //         vec{src_coords[2]},
                //         projected_bc,
                //         d
                //     );

                //     *out_it++ = {1 - projected_bc.x - projected_bc.y, projected_bc};
                // }
                // else
                // {
                //     using vec3 = glm::vec<3, T, Q>;

                //     vec projected_bc;

                //     glm::intersectRayTriangle(
                //         vec3{},
                //         vec3{src_p},
                //         vec3{src_coords[0]},
                //         vec3{src_coords[0]},
                //         vec3{src_coords[1]},
                //         projected_bc,
                //         d
                //     );

                //     *out_it++ = projected_bc;
                // }
            }
        }
    } trivial_perspective_interpolate{};

    inline constexpr struct perspective_interpolate_fn
    {
        template<
            typename T,
            glm::qualifier Q,
            glm::length_t L,
            std::ranges::input_range BarycentricCoordRng,
            typename OutIt>
            requires std::same_as<
                         std::decay_t<std::ranges::range_value_t<BarycentricCoordRng>>,
                         glm::vec<L, T, Q>> &&
            (L > 1)
        constexpr auto operator()(
            const glm::vec<L, T, Q>& depth_coords,
            BarycentricCoordRng&& barycentric_coords,
            OutIt out_it
        ) const
        {
            for(auto coord : cpp_forward(barycentric_coords))
            {
                coord /= depth_coords;
                *out_it++ =
                    coord / ::ranges::accumulate(coord, typename decltype(coord)::value_type{0});
            }
        }
    } perspective_interpolate{};
}