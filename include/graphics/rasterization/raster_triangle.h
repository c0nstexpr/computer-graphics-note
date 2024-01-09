#pragma once

#include <algorithm>

#include <stdsharp/utility/auto_cast.h>

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
            using star::auto_cast;

            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = -f0_a * p0.x - f0_b * p0.y;

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = -f1_a * p1.x - f1_b * p1.y;

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = -f2_a * p2.x - f2_b * p2.y;

            const OutT f0_x2y2 = auto_cast(f0_a * p2.x + f0_b * p2.y + f0_c);
            const OutT f1_x0y0 = auto_cast(f1_a * p0.x + f1_b * p0.y + f1_c);
            const OutT f2_x1y1 = auto_cast(f2_a * p1.x + f2_b * p1.y + f2_c);

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            for(auto y = py.min; y <= py.max; ++y)
                for(auto x = px.min; x <= px.max; ++x)
                {
                    const auto a = (f1_a * x + f1_b * y + f1_c) / f1_x0y0;
                    const auto b = (f2_a * x + f2_b * y + f2_c) / f2_x1y1;
                    const auto c = (f0_a * x + f0_b * y + f0_c) / f0_x2y2;

                    if(a <= 0 || b <= 0 || c <= 0) continue;

                    *out_p++ = glm::vec<2, T, Q>{x, y};
                    *out_bcoor++ = glm::vec<3, OutT, OutQ>{a, b, c};
                }
        }
    };

    template<typename OutT = float, glm::qualifier OutQ = glm::defaultp>
    inline constexpr trivial_raster_triangle_fn<OutT, OutQ> trivial_raster_triangle{};

    template<typename OutT = float, glm::qualifier OutQ = glm::defaultp>
    struct raster_triangle_fn
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
            using star::auto_cast;

            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = -f0_a * p0.x - f0_b * p0.y;

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = -f1_a * p1.x - f1_b * p1.y;

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = -f2_a * p2.x - f2_b * p2.y;

            const OutT f0_x2y2 = auto_cast(f0_a * p2.x + f0_b * p2.y + f0_c);
            const OutT f1_x0y0 = auto_cast(f1_a * p0.x + f1_b * p0.y + f1_c);
            const OutT f2_x1y1 = auto_cast(f2_a * p1.x + f2_b * p1.y + f2_c);

            const auto f0_dx = f0_a / f1_x0y0;
            const auto f1_dx = f1_a / f2_x1y1;
            const auto f2_dx = f2_a / f0_x2y2;

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            const auto diff_x = px.max - px.min;

            const auto f0_dy = (f0_b - f0_a * diff_x) / f1_x0y0;
            const auto f1_dy = (f1_b - f1_a * diff_x) / f2_x1y1;
            const auto f2_dy = (f2_b - f2_a * diff_x) / f0_x2y2;

            OutT f0_xy = auto_cast(f0_a * px.min + f0_b * py.min + f0_c);
            OutT f1_xy = auto_cast(f1_a * px.min + f1_b * py.min + f1_c);
            OutT f2_xy = auto_cast(f2_a * px.min + f2_b * py.min + f2_c);

            for(auto y = py.min; y <= py.max; ++y, f0_xy += f0_dy, f1_xy += f1_dy, f2_xy += f2_dy)
            {
                auto x = px.min;

                // triangle is convex
                for(; x <= px.max && (f0_xy <= 0 || f1_xy <= 0 || f2_xy <= 0);
                    ++x, f0_xy += f0_dx, f1_xy += f1_dx, f2_xy += f2_dx)
                {
                }

                for(; x <= px.max && f0_xy > 0 && f1_xy > 0 && f2_xy > 0;
                    ++x, f0_xy += f0_dx, f1_xy += f1_dx, f2_xy += f2_dx)
                {
                    *out_p++ = glm::vec<2, T, Q>{x, y};
                    *out_bcoor++ = glm::vec<3, OutT, OutQ>{f0_xy, f1_xy, f2_xy};
                }
            }
        }
    };

    template<typename OutT = float, glm::qualifier OutQ = glm::defaultp>
    inline constexpr raster_triangle_fn<OutT, OutQ> raster_triangle{};
}