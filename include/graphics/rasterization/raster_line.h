#pragma once

#include <compare>
#include <iterator>

#include <stdsharp/cassert/cassert.h>
#include <stdsharp/compare/compare.h>

#include "../namespace_alias.h"

namespace graphics::rasterization::details
{
    template<typename Impl>
    struct raster_line_fn
    {
        template<typename T, glm::qualifier Qualifier>
            requires(!star::unsigned_<T>)
        constexpr auto operator()(
            glm::vec<2, T, Qualifier> p0,
            const decltype(p0) p1,
            std::output_iterator<decltype(p0)> auto out_it,
            decltype(p0) d = {}
        ) const
        {
            const auto diff = p1 - p0;
            const auto x_diff_cmp = diff.x <=> 0;
            const auto y_diff_cmp = diff.y <=> 0;

            Expects(is_eq(x_diff_cmp) || star::conform_to(d.x <=> 0, x_diff_cmp));
            Expects(is_eq(y_diff_cmp) || star::conform_to(d.y <=> 0, y_diff_cmp));

            if(d.x == 0) d.x = diff.x > 0 ? 1 : -1;
            if(d.y == 0) d.y = diff.y > 0 ? 1 : -1;

            if(diff.x == 0)
            {
                for(; star::conform_to(p1.y <=> p0.y, y_diff_cmp); p0.y += d.y, ++out_it)
                    *out_it = p0;
                return;
            }

            if(diff.y == 0)
            {
                for(; star::conform_to(p1.x <=> p0.x, x_diff_cmp); p0.x += d.x, ++out_it)
                    *out_it = p0;
                return;
            }

            Impl{}(p0, p1, out_it, d, diff, x_diff_cmp, y_diff_cmp);
        }
    };

    struct trivial_raster_line_fn
    {
        constexpr auto operator()(
            auto& p0,
            const auto& p1,
            auto& out_it,
            const auto& d,
            const auto& diff,
            const auto& x_diff_cmp,
            const auto& y_diff_cmp
        ) const
        {
            // a = diff.y;
            // b = -diff.x;
            const auto c = diff.y * p0.x - diff.x * p0.y;
            const auto c_dy = -diff.x * d.y;

            for(; star::conform_to(p1.x <=> p0.x, x_diff_cmp); p0.x += d.x)
            {
                auto current_c = diff.y * p0.x - diff.x * p0.y;
                if(abs(current_c - c) >= abs(current_c + c_dy - c)) do // NOLINT(*-do-while)
                    {
                        p0.y += d.y;
                        if(!star::conform_to(p1.y <=> p0.y, y_diff_cmp)) return;

                        *(out_it++) = p0;

                        current_c += c_dy;
                    } while(abs(current_c - c) >= abs(current_c + c_dy - c));
                else *(out_it++) = p0;
            }
        }
    };

    struct bresenham_raster_line_fn
    {
        template<typename T, glm::qualifier Q>
        constexpr auto operator()(
            glm::vec<2, T, Q>& p0,
            const auto& p1,
            auto& out_it,
            const auto& d,
            const auto& diff,
            const auto& x_diff_cmp,
            const auto& y_diff_cmp
        ) const
        {
            // a = diff.y;
            // b = -diff.x;
            // delta_c = a * d.x + b * d.y
            const auto c2_dx = 2 * diff.y * d.x;
            const auto c_dy = -diff.x * d.y;
            const auto c2_dy = 2 * c_dy;
            const auto c_cmp = c2_dx <=> 0;

            // a * (x + d.x) + b * y
            for(T c2 = 0; star::conform_to(p1.x <=> p0.x, x_diff_cmp); p0.x += d.x, c2 += c2_dx)
                // current_c = a * (x + d.x * i) + b * (y + d.y * j)
                // c - current_c = b * d.y * k
                if(star::conform_to(c2 + c_dy <=> 0, c_cmp)) do // NOLINT(*-do-while)
                    {
                        p0.y += d.y;
                        if(!star::conform_to(p1.y <=> p0.y, y_diff_cmp)) return;

                        *(out_it++) = p0;

                        c2 += c2_dy;
                    } while(star::conform_to(c2 + c_dy <=> 0, c_cmp));
                else *(out_it++) = p0;
        }
    };
}

namespace graphics::rasterization
{
    using trivial_raster_line_fn = details::raster_line_fn<details::trivial_raster_line_fn>;

    inline constexpr trivial_raster_line_fn trivial_raster_line{};

    using bresenham_raster_line_fn = details::raster_line_fn<details::bresenham_raster_line_fn>;

    inline constexpr bresenham_raster_line_fn bresenham_raster_line{};
}