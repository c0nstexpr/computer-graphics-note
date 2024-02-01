#pragma once

#include <compare>
#include "../namespace_alias.h"

namespace graphics::rasterization
{
    struct clip_fn
    {
        template<typename T, glm::qualifier Q, glm::length_t L>
        constexpr std::pair<glm::vec<L, T, Q>, T> operator()(
            const glm::vec<L, T, Q>& plane_normal,
            const T& plane_constant,
            glm::mat<2, L, T, Q> line
        ) const noexcept
        {
            const auto& a = line[0];
            const auto& b = line[1];

            const auto a_cmp = plane_normal * a + plane_constant >= 0;
            const auto b_cmp = plane_normal * b + plane_constant >= 0;

            return a_cmp != b_cmp ?
                std::pair{
                    a_cmp ? a : b,
                    (glm::dot(plane_normal, a) + plane_constant) / glm::dot(plane_normal, a - b)
                } :
                a_cmp ? std::pair{a, static_cast<T>(1)} :
                        std::pair{a, static_cast<T>(-1)};
        }
    };
}