#pragma once

#include <compare>
#include <iterator>

#include <stdsharp/cassert/cassert.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    inline constexpr struct bresenham_draw_line_fn
    {
    private:
        template<typename T>
        static constexpr bool is_followed(const T l, const T r)
        {
            return is_eq(l) || (l == r);
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
            const auto diff = p1 - p0;
            const auto x_diff_cmp = diff.x <=> 0;
            const auto y_diff_cmp = diff.y <=> 0;

            Expects(is_followed(dx <=> 0, x_diff_cmp));
            Expects(is_followed(dy <=> 0, y_diff_cmp));

            if(dx == 0) dx = diff.x > 0 ? 1 : -1;
            if(dy == 0) dy = diff.y > 0 ? 1 : -1;

            if(diff.x == 0)
            {
                for(; is_followed(p1.y <=> p0.y, y_diff_cmp); p0.y += dy, ++out_it) *out_it = p0;
                return;
            }

            if(diff.y == 0)
            {
                for(; is_followed(p1.x <=> p0.x, x_diff_cmp); p0.x += dx, ++out_it) *out_it = p0;
                return;
            }

            // a = diff.y;
            // b = -diff.x;
            // delta_c = a * dx + b * dy
            const auto c2_dx = 2 * diff.y * dx;
            const auto c_dy = -diff.x * dy;
            const auto c2_dy = 2 * c_dy;
            const auto c_cmp = c2_dx <=> 0;

            // a * (x + dx) + b * y
            for(T c2 = 0; is_followed(p1.x <=> p0.x, x_diff_cmp); p0.x += dx, c2 += c2_dx)
                // current_c = a * (x + dx * i) + b * (y + dy * j)
                // c - current_c = b * dy * k
                if(is_followed(c2 + c_dy <=> 0, c_cmp)) do // NOLINT(*-do-while)
                    {
                        p0.y += dy;
                        if(!is_followed(p1.y <=> p0.y, y_diff_cmp)) return;

                        *(out_it++) = p0;

                        c2 += c2_dy;
                    } while(is_followed(c2 + c_dy <=> 0, c_cmp));
                else *(out_it++) = p0;
        }
    } bresenham_draw_line{};
}