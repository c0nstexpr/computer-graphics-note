#pragma once

#include <algorithm>

#include <glm/gtx/intersect.hpp>

#include <stdsharp/cassert/cassert.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    inline constexpr struct line_clip_fn
    {
        template<typename T, glm::qualifier Q, glm::length_t L>
        [[nodiscard]] constexpr std::pair<glm::vec<L, T, Q>, T> operator()(
            glm::vec<L, T, Q> plane_normal,
            const glm::vec<L, T, Q>& plane_p,
            const glm::mat<2, L, T, Q>& line
        ) const noexcept
        {
            auto& a = line[0];
            auto& b = line[1];
            const auto& normal_dot_p = glm::dot(plane_normal, plane_p);
            const auto& normal_dot_a = glm::dot(plane_normal, a);
            const auto& normal_dot_b = glm::dot(plane_normal, b);

            const auto a_cmp = normal_dot_a >= normal_dot_p;
            const auto b_cmp = normal_dot_b >= normal_dot_p;

            return a_cmp != b_cmp ?
                std::pair{a_cmp ? a : b, (normal_dot_p - normal_dot_a) / (normal_dot_b - normal_dot_a)} :
                a_cmp ? std::pair{a, static_cast<T>(1)} :
                        std::pair{a, static_cast<T>(-1)};
        }

        template<typename T, glm::qualifier Q, glm::length_t L>
        [[nodiscard]] constexpr std::pair<glm::vec<L, T, Q>, T> operator()(
            const glm::vec<L, T, Q>& plane_normal,
            const T& plane_constant,
            const glm::mat<2, L, T, Q>& line
        ) const noexcept
        {
            const auto& a = line[0];
            const auto& b = line[1];
            const auto& plane_as_a = glm::dot(plane_normal, a) + plane_constant;
            const auto& plane_as_b = glm::dot(plane_normal, b) + plane_constant;

            const auto a_cmp = plane_as_a >= 0;
            const auto b_cmp = plane_as_b >= 0;

            return a_cmp != b_cmp ?
                std::pair{a_cmp ? a : b, plane_as_a / (plane_as_a - plane_as_b)} :
                a_cmp ? std::pair{a, static_cast<T>(1)} :
                        std::pair{a, static_cast<T>(-1)};
        }
    } line_clip{};
}