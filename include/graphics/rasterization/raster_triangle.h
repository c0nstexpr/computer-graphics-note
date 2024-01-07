#pragma once

#include <algorithm>
#include <iterator>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    template<typename OutT = float, glm::qualifier OutQ = glm::defaultp>
    struct trivial_raster_triangle_fn
    {
        template<typename T, glm::qualifier Q>
        constexpr void operator()(
            const glm::vec<2, T, Q>& p0,
            decltype(p0) p1,
            decltype(p0) p2,
            std::output_iterator<glm::vec<2, T, Q>> auto out_p,
            std::output_iterator<glm::vec<3, OutT, OutQ>> auto out_bcoor
        ) const
        {
            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = p0.x * p1.y - p1.x * p0.y; // x0y1 - x1y0

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = p1.x * p2.y - p2.x * p1.y; // x1y2 - x2y1

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = p2.x * p0.y - p0.x * p2.y; // x2y0 - x0y2

            const auto f0_x2y2 = static_cast<OutT>(f0_a * p2.x + f0_b * p2.y + f0_c);
            const auto f1_x0y0 = static_cast<OutT>(f1_a * p0.x + f1_b * p0.y + f1_c);
            const auto f2_x1y1 = static_cast<OutT>(f2_a * p1.x + f2_b * p1.y + f2_c);

            for(auto y = py.min; y <= py.max; ++y)
                for(auto x = px.min; x <= px.max; ++x)
                    if(const T a = (f1_a * x + f1_b * y + f1_c) / f1_x0y0,
                       b = (f2_a * x + f2_b * y + f2_c) / f2_x1y1,
                       c = (f0_a * x + f0_b * y + f0_c) / f0_x2y2;
                       a >= 0 && b >= 0 && c >= 0)
                    {
                        *out_p++ = glm::vec<2, T, Q>{x, y};
                        *out_bcoor++ = glm::vec<3, OutT, OutQ>{a, b, c};
                    }
        }
    };

    template<typename OutT = float, glm::qualifier OutQ = glm::defaultp>
    inline constexpr trivial_raster_triangle_fn<OutT, OutQ> trivial_raster_triangle{};
}