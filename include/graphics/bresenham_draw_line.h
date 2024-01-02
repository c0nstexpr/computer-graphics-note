#pragma once

#include <stdsharp/cassert/cassert.h>

#include "namespace_alias.h"

namespace graphics
{
    inline constexpr struct bresenham_draw_line_fn
    {
        template<typename T, glm::qualifier Qualifier>
        constexpr auto operator()(
            glm::vec<2, T, Qualifier> p0,
            const decltype(p0) p1,
            std::output_iterator<decltype(p1)> auto out_it,
            const T dx,
            const T dy
        ) const
        {
            const auto line_a = p0.y - p1.y;
            const auto line_b = p1.x - p0.x;

            Expects((dx <=> 0) == (line_b <=> 0));
            Expects((-dy <=> 0) == (line_a <=> 0));

            const auto c_dy = line_b * dy;

            if(const auto c_dy_cmp = c_dy <=> 0; c_dy_cmp != std::strong_ordering::equal)
            {
                const auto line_c = line_a * p0.x + line_b * p0.y;
                const auto c_dx = line_a * dx;
                const auto two_line_c = 2 * line_c;

                // a * (x + dx) + b * y
                for(auto c = line_c; dx >= 0 ? p0.x <= p1.x : p0.x >= p1.x; p0.x += dx, c += c_dx)
                {
                    *(out_it++) = p0;

                    // a * (x + dx) + b * (y + dy)
                    for(auto c_next = c + c_dy; (c_next + c <=> two_line_c) == c_dy_cmp;
                        ++out_it, p0.y += dy, c = c_next, c_next += c_dy)
                        *out_it = p0;
                }
            }
            else
                for(; dx >= 0 ? p0.x <= p1.x : p0.x >= p1.x; p0.x += dx, ++out_it) *out_it = p0;
        }

        template<typename T, glm::qualifier Qualifier>
        constexpr auto operator()(
            glm::vec<2, T, Qualifier> p0,
            const decltype(p0) p1,
            std::output_iterator<decltype(p1)> auto out_it
        ) const
        {
            return (*this)(p0, p1, out_it, p1.x > p0.x ? 1 : -1, p1.y > p0.y ? 1 : -1);
        }
    } bresenham_draw_line{};
}