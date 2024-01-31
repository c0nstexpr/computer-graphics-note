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
            const glm::mat<L - 1, L, T, Q>& homo_coords,
            std::type_identity_t<glm::mat<L, L, T, Q>> trans,
            BarycentricCoordRng&& barycentric_coords,
            OutIt out_it
        ) const
        {
            constexpr auto dimension = L - 1;
            using vec = glm::vec<dimension, T, Q>;

            glm::mat<dimension, dimension, T, Q> src_coords;
            glm::mat<dimension, L, T, Q> dst_barycentric{};

            {
                glm::mat<dimension, L, T, Q> dst_coords{};
                trans = glm::inverse(trans);

                for(glm::length_t i = 0; i < dimension; ++i)
                {
                    const auto dst_homo = homo_coords[i];

                    {
                        const auto src_homo = trans * dst_homo;
                        src_coords[i] = vec{src_homo / src_homo[dimension]};
                    }

                    dst_coords[i] = dst_homo / dst_homo[dimension];
                }

                dst_barycentric = trans * dst_coords;
            }

            for(const auto barycentric_inv = glm::inverse(src_coords);
                const auto& barycentric : cpp_forward(barycentric_coords))
            {
                const auto homo_src_p = dst_barycentric * barycentric;
                *out_it++ = barycentric_inv * static_cast<vec>(homo_src_p / homo_src_p[dimension]);
            }
        }
    } trivial_perspective_interpolate{};

    inline constexpr struct depth_mul_perspective_interpolate_fn
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
            glm::vec<L, T, Q> depth_factors{1};

            for(glm::length_t i = 0; i < L; ++i)
            {
                glm::length_t j = 0;
                for(; j < i; ++j) depth_factors[i] *= depth_coords[j];
                for(++j; j < L; ++j) depth_factors[i] *= depth_coords[j];
            }

            for(auto coord : cpp_forward(barycentric_coords))
            {
                coord *= depth_factors;

                typename decltype(coord)::value_type sum = 0;

                for(glm::length_t i = 0; i < L; ++i) sum += coord[i];

                *out_it++ = coord / sum;
            }
        }
    } depth_mul_perspective_interpolate{};

    inline constexpr struct depth_div_perspective_interpolate_fn
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

                typename decltype(coord)::value_type sum = 0;

                for(glm::length_t i = 0; i < L; ++i) sum += coord[i];

                *out_it++ = coord / sum;
            }
        }
    } depth_div_perspective_interpolate{};
}