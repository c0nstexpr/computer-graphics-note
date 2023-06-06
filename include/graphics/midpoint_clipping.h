#pragma once

#include <glm/detail/qualifier.hpp>
#include <stdsharp/cassert/cassert.h>

#include "namespace_alias.h"

namespace graphics
{
    namespace details
    {
        constexpr bool reject(const glm::bvec4 code)
        {
            for(glm::length_t i = 0; i < 4; ++i)
                if(!code[i]) return true;

            return false;
        }

        template<glm::qualifier Qualifier>
        constexpr auto midpoint_approach(
            glm::vec<2, double, Qualifier> p0,
            glm::vec<2, double, Qualifier> p1, // NOLINT
            const glm::vec<2, double, Qualifier> win_min,
            const glm::vec<2, double, Qualifier> win_max
        )
        {
            while(p0 != p1)
            {
                const auto mid_p = (p0 + p1) / 2.;

                if(all(greaterThanEqual(mid_p, win_min)) && all(lessThanEqual(mid_p, win_max)))
                {
                    if(p1 == mid_p)
                    {
                        p0 = mid_p;
                        break;
                    }

                    p1 = mid_p;
                }
                else if(p0 == mid_p) break;
                else p0 = mid_p;
            }

            return p0;
        }
    }

    // TODO: use integral types and minimize errors
    template<typename T = int, glm::qualifier Qualifier = glm::qualifier::defaultp>
        requires std::is_signed_v<T>
    constexpr auto midpoint_clipping(
        glm::vec<2, T, Qualifier> p0,
        glm::vec<2, std::type_identity_t<T>, Qualifier> p1,
        const glm::vec<2, double, Qualifier> win_min,
        const glm::vec<2, double, Qualifier> win_max
    )
    {
        star::precondition<std::invalid_argument>(
            [=] { return all(lessThanEqual(win_min, win_max)); },
            "incorrect points input, min should be strictly less than max"
        );

        struct clipped_t
        {
            glm::vec<2, double, Qualifier> p0;
            glm::vec<2, double, Qualifier> p1;
        };

        glm::vec<2, double, Qualifier> d_p0 = p0;
        glm::vec<2, double, Qualifier> d_p1 = p1;

        glm::bvec4 outcode0{greaterThanEqual(d_p0, win_min), lessThanEqual(d_p0, win_max)};
        glm::bvec4 outcode1{greaterThanEqual(d_p1, win_min), lessThanEqual(d_p1, win_max)};

        // reject the line when both on the same side of window
        if(details::reject(outcode0 | outcode1))
            return clipped_t{{std::numeric_limits<double>::quiet_NaN(), 0}};

        // find the intersections
        glm::vec<2, double, Qualifier> mid_p;

        // cut the line until the mid point is inside the window
        while(true)
        {
            mid_p = (d_p0 + d_p1) / 2.; // NOLINT

            const glm::bvec4 code_mid{
                greaterThanEqual(mid_p, win_min),
                lessThanEqual(mid_p, win_max) //
            };

            if(details::reject(code_mid | outcode0))
            {
                d_p0 = mid_p;
                outcode0 = code_mid;
            }
            else if(details::reject(code_mid | outcode1))
            {
                d_p1 = mid_p;
                outcode1 = code_mid;
            }
            else break;
        }

        // find intersections by midpoint approaching
        return clipped_t{
            all(outcode0) ? d_p0 : details::midpoint_approach(d_p0, mid_p, win_min, win_max),
            all(outcode1) ? d_p1 : details::midpoint_approach(d_p1, mid_p, win_min, win_max) //
        };
    }
}