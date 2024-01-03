#pragma once

#include <compare>
#include <iterator>

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
            T dx,
            T dy
        ) const
        {
            const auto line_b = p1.x - p0.x;
            const auto line_a = p0.y - p1.y;

            Expects((-dy <=> 0) == (line_a <=> 0));
            Expects((dx <=> 0) == (line_b <=> 0));

            if(dx == 0) dx = line_b > 0 ? 1 : -1;
            if(dy == 0) dy = line_a < 0 ? 1 : -1;

            if(line_b == 0)
            {
                if(line_a <= 0)
                    for(; p0.y <= p1.y; p0.y += dy, ++out_it) *out_it = p0;
                else
                    for(; p0.y >= p1.y; p0.y += dy, ++out_it) *out_it = p0;
                return;
            }


            const auto line_c = line_a * p0.x + line_b * p0.y;
            const auto c_dy = line_b * dy;
            const auto c_dx = line_a * dx;

            const auto c_dy_cmp = c_dy <=> 0;
            const auto half_c_dy = c_dy / 2;
            const auto two_line_c = 2 * line_c;

            // a * (x + dx) + b * y
            for(auto c = line_c; dx >= 0 ? p0.x <= p1.x : p0.x >= p1.x;
                p0.x += dx, ++out_it, c += c_dx)
            {
                // current_c = a * (x + dx * i) + b * (y + dy * j)
                // current_c + b * dy * k - c <=> c - current_c
                // c <=> current_c
                const auto c_diff = line_c >= c ? //
                    (is_gteq(c_dy_cmp) ? line_c - c : 0) :
                    (is_lt(c_dy_cmp) ? c - line_c : 0);

                if(c_diff >= half_c_dy) // div and rounding
                    if(const auto step = (c_diff + half_c_dy) / c_dy; step != 0)
                    {
                        p0.y += dy * step;
                        c += c_dy * step;
                    }

                *out_it = p0;
            }
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