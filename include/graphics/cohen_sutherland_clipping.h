#pragma once

#include <algorithm>
#include <glm/fwd.hpp>
#include <optional>

#include <stdsharp/cassert/cassert.h>

#include "namespace_alias.h"

namespace graphics
{
    namespace details
    {
        template<typename T, glm::qualifier Qualifier>
        constexpr void cohen_sutherland_intersect(
            const glm::vec<3, T, Qualifier> factor,
            const glm::vec<2, double, Qualifier> win_min,
            const glm::vec<2, double, Qualifier> win_max,
            glm::vec<2, double, Qualifier>& p,
            glm::bvec4& code,
            bool& in_window
        )
        {
            if(in_window) return;

            if(!code[0]) p = {win_min.x, -(factor[2] + factor[0] * win_min.x) / factor[1]};
            else if(!code[1]) p = {-(factor[2] + factor[1] * win_min.y) / factor[0], win_min.y};
            else if(!code[2]) p = {win_max.x, -(factor[2] + factor[0] * win_max.x) / factor[1]};
            else p = {-(factor[2] + factor[1] * win_max.y) / factor[0], win_max.y};

            code = {greaterThanEqual(p, win_min), lessThanEqual(p, win_max)};
            in_window = all(code);
        }
    }

    template<glm::qualifier Qualifier = glm::qualifier::defaultp>
    struct cohen_sutherland_clipped_t
    {
        glm::vec<2, double, Qualifier> p0;
        glm::vec<2, double, Qualifier> p1;
    };

    template<typename T = int, glm::qualifier Qualifier = glm::qualifier::defaultp>
        requires std::is_signed_v<T>
    constexpr cohen_sutherland_clipped_t<Qualifier> cohen_sutherland_clipping(
        const glm::vec<2, T, Qualifier> p0,
        const glm::vec<2, T, Qualifier> p1,
        const glm::vec<2, double, Qualifier> win_min,
        const glm::vec<2, double, Qualifier> win_max
    )
    {
        star::precondition<std::invalid_argument>(
            [=] { return all(lessThanEqual(win_min, win_max)); },
            "incorrect points input, min should be strictly less than max"
        );

        const auto line_a = p1.y - p0.y;
        const auto line_b = p0.x - p1.x;

        if(line_a == 0) return {{win_min.x, p0.y}, {win_max.x, p1.y}};
        if(line_b == 0) return {{p0.x, win_min.y}, {p1.x, win_max.y}};

        cohen_sutherland_clipped_t<Qualifier> clipped = {p0, p1};

        glm::bvec4 outcode0{
            greaterThanEqual(clipped.p0, win_min),
            lessThanEqual(clipped.p0, win_max)};
        glm::bvec4 outcode1{
            greaterThanEqual(clipped.p1, win_min),
            lessThanEqual(clipped.p1, win_max)};

        // reject the line when both on the same side of window
        {
            const auto or_res = outcode0 | outcode1;

            for(glm::length_t i = 0; i < 4; ++i)
                if(!or_res[i])
                {
                    constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
                    constexpr glm::vec<2, double, Qualifier> nan_vec{nan, nan};
                    return {nan_vec, nan_vec};
                }
        }

        const glm::vec<3, T, Qualifier> factor{line_a, line_b, p1.x * p0.y - p0.x * p1.y};

        // detect the points region
        // accept the line when both in window
        for(bool in_win0 = all(outcode0), in_win1 = all(outcode1); !(in_win0 && in_win1);)
        {
            // find the intersections
            details::cohen_sutherland_intersect(
                factor,
                win_min,
                win_max,
                clipped.p0,
                outcode0,
                in_win0
            );
            details::cohen_sutherland_intersect(
                factor,
                win_min,
                win_max,
                clipped.p1,
                outcode1,
                in_win1
            );
        }


        return clipped;
    }
}