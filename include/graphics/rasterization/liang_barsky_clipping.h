#pragma once

#include <stdsharp/algorithm/algorithm.h>
#include <stdsharp/cassert/cassert.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    namespace details
    {
        template<typename T>
        constexpr void
            liang_barsky_parallel(double& v0, double& v1, const T min, const T max) // NOLINT
        {
            if(v0 < min)
            {
                if(v1 < min)
                {
                    v0 = min - 1;
                    return;
                }

                v0 = min;
                star::set_if_less(v1, max);
            }
            else if(v0 > max)
            {
                if(v1 > max)
                {
                    v0 = min - 1;
                    return;
                }

                v0 = max;
                star::set_if_greater(v1, min);
            }
            else if(v1 < min) v1 = min;
            else if(v1 > max) v1 = max;
        }

        template<typename T, glm::qualifier Qualifier>
        constexpr auto liang_barsky_intersect(
            const glm::vec<2, T, Qualifier> p0,
            const glm::vec<2, T, Qualifier> p1, // NOLINT
            const glm::vec<2, T, Qualifier> win_min,
            const glm::vec<2, T, Qualifier> win_max,
            const T dx,
            const T dy,
            auto& ret
        )
        {
            double u_out_to_in{};
            double u_in_to_out{};

            {
                using dvec2 = glm::vec<2, double, Qualifier>;
                using dvec4 = glm::vec<4, double, Qualifier>;

                const dvec4 p_p0{-dx, dx, -dy, dy};
                const auto u = // get the intersection params
                    dvec4{
                        p0.x - win_min.x,
                        win_max.x - p0.x,
                        p0.y - win_min.y,
                        win_max.y - p0.y //
                    } /
                    p_p0;

                // get direction by sign
                const auto ux = p_p0[0] < 0 ? dvec2{u[0], u[1]} : dvec2{u[1], u[0]};
                const auto uy = p_p0[2] < 0 ? dvec2{u[2], u[3]} : dvec2{u[3], u[2]};

                u_out_to_in = std::max(ux[0], uy[0]);
                u_in_to_out = std::min(ux[1], uy[1]);
            }

            star::set_if_greater(u_out_to_in, 0);
            star::set_if_less(u_in_to_out, 1);

            // out of window?
            if(u_out_to_in > u_in_to_out) return;

            // substitute the param
            ret = {
                {p0.x + u_out_to_in * dx, p0.y + u_out_to_in * dy},
                {p0.x + u_in_to_out * dx, p0.y + u_in_to_out * dy} //
            };
        }
    }

    template<typename T = int, glm::qualifier Qualifier = glm::qualifier::defaultp>
        requires std::is_signed_v<T>
    constexpr auto liang_barsky_clipping(
        const glm::vec<2, T, Qualifier> p0,
        const glm::vec<2, std::type_identity_t<T>, Qualifier> p1,
        const glm::vec<2, std::type_identity_t<T>, Qualifier> win_min,
        const glm::vec<2, std::type_identity_t<T>, Qualifier> win_max
    )
    {
        Expects(all(lessThanEqual(win_min, win_max)));

        struct clipped_t
        {
            glm::vec<2, double, Qualifier> p0;
            glm::vec<2, double, Qualifier> p1;
        };

        clipped_t ret{};
        const auto invalid_v = win_min.x - 1;
        const auto dx = p1.x - p0.x;
        const auto dy = p1.y - p0.y;

        // parallel to one of the window edges
        if(dx == 0)
        {
            ret = {{p0.x, p0.y}, {p0.x, p1.y}};
            details::liang_barsky_parallel(ret.p0.y, ret.p1.y, win_min.y, win_max.y);
            if(ret.p0.y < win_min.y) ret.p0.x = invalid_v;
        }
        else if(dy == 0)
        {
            ret = {{p0.x, p0.y}, {p1.x, p0.y}};
            details::liang_barsky_parallel(ret.p0.x, ret.p1.x, win_min.x, win_max.x);
            if(ret.p0.x < win_min.x) ret.p0.x = invalid_v;
        }
        else // get intersections
        {
            ret.p0.x = invalid_v;
            details::liang_barsky_intersect(p0, p1, win_min, win_max, dx, dy, ret);
        }

        return ret;
    }
}