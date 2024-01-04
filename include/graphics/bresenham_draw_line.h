#pragma once

#include <compare>
#include <iterator>

#include <stdsharp/cassert/cassert.h>

#include "namespace_alias.h"

namespace graphics
{
    inline constexpr struct bresenham_draw_line_fn
    {
    private:
        template<typename T, glm::qualifier Qualifier>
        static constexpr auto impl(
            glm::vec<2, T, Qualifier> p0,
            const T line_a,
            const T line_b,
            auto out_it,
            T dx,
            T dy
        )
        {
            const auto line_c = line_a * p0.x + line_b * p0.y;
            const auto c_dy = line_b * dy;
            const auto c_dx = line_a * dx;

            auto x_diff = line_b;
            auto y_diff = -line_a;

            const auto c_dy_cmp = c_dy <=> 0;
            const auto half_c_dy = c_dy / 2;
            const auto two_line_c = 2 * line_c;

            // a * (x + dx) + b * y
            for(auto c = line_c; std::abs(x_diff) > std::abs(dx);
                x_diff -= dx, p0.x += dx, c += c_dx, ++out_it)
            {
                if(y_diff != 0)
                {
                    // current_c = a * (x + dx * i) + b * (y + dy * j)
                    // b * dy * k = current_c - c
                    if(const auto step = (c - line_c + half_c_dy) / c_dy; step != 0)
                    {
                        auto d = dy * step;
                        if(std::abs(y_diff) <= std::abs(d)) d = y_diff;

                        p0.y += d;
                        y_diff -= d;
                        c += d * line_b;
                    }
                }

                *out_it = p0;
            }

            if(std::abs(x_diff) > 0)
            {
                p0.x += x_diff;
                p0.y += y_diff;
                *out_it = p0;
            }
        }

    public:
        template<typename T, glm::qualifier Qualifier>
        constexpr auto operator()(
            glm::vec<2, T, Qualifier> p0,
            const decltype(p0) p1,
            std::output_iterator<decltype(p1)> auto out_it,
            T dx = 0,
            T dy = 0
        ) const
        {
            const auto line_a = p0.y - p1.y;
            const auto line_b = p1.x - p0.x;

            Expects((-dy <=> 0) == (line_a <=> 0));
            Expects((dx <=> 0) == (line_b <=> 0));

            if(dy == 0) dy = line_a < 0 ? 1 : -1;

            if(line_b == 0)
            {
                if(line_a < 0)
                    for(; p0.y <= p1.y; p0.y += dy, ++out_it) *out_it = p0;
                else if(line_a > 0)
                    for(; p0.y >= p1.y; p0.y += dy, ++out_it) *out_it = p0;
                else *out_it = p0;

                return;
            }

            if(dx == 0) dx = line_b > 0 ? 1 : -1;

            if(line_a == 0)
            {
                if(line_b > 0)
                    for(; p0.x <= p1.x; p0.x += dx, ++out_it) *out_it = p0;
                else if(line_b < 0)
                    for(; p0.x >= p1.x; p0.x += dx, ++out_it) *out_it = p0;

                return;
            }

            impl(p0, line_a, line_b, out_it, dx, dy);
        }
    } bresenham_draw_line{};
}