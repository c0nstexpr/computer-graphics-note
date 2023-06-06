#pragma once

#include <algorithm>
#include <cstdlib>
#include <cinttypes>

#include <stdsharp/cassert/cassert.h>
#include <stdsharp/utility/auto_cast.h>

#include "namespace_alias.h"

namespace graphics
{
    namespace details
    {
        template<typename T, glm::qualifier Qualifier, typename OutIt>
        constexpr OutIt bresenham_draw_line(
            glm::vec<2, T, Qualifier>& p0,
            std::make_unsigned_t<T>& d,
            const glm::vec<2, ::std::type_identity_t<T>, Qualifier> diff,
            OutIt out_it
        )
        {
            const auto div_res = div(d + diff.x, 2 * diff.x);
            const auto quot = div_res.quot;

            d = div_res.rem - diff.x;

            if(quot == 0)
            {
                *out_it++ = p0;
                return out_it;
            }

            return std::ranges::generate_n(
                out_it,
                star::auto_cast(quot),
                [&p0]
                {
                    ++p0.y;
                    return p0;
                }
            );
        }
    }

    template<
        typename T = int,
        glm::qualifier Qualifier = glm::qualifier::defaultp,
        std::output_iterator<glm::vec<2, T, Qualifier>> OutIt // clang-format off
    > // clang-format on
        requires std::is_signed_v<T>
    constexpr OutIt bresenham_draw_line(
        glm::vec<2, T, Qualifier> p0,
        const glm::vec<2, T, Qualifier> p1,
        OutIt out_it,
        const std::make_unsigned_t<T> dx = 1
    )
    {
        star::precondition<std::invalid_argument>(
            [&] { return p0.x <= p1.x; },
            "incorrect points order, p0 should be on the left of p1"
        );

        if(p0.x == p1.x)
        {
            for(; p0.y < p1.y; ++p0.y) *out_it++ = p0;
            return out_it;
        }

        const auto diff = p1 - p0;
        const auto dy = 2 * diff.y * dx;

        *out_it++ = p0;
        p0.x += dx;

        for(auto d = dy; p0.x <= p1.x; p0.x += dx, d += dy)
            out_it = details::bresenham_draw_line(p0, d, diff, out_it);

        return out_it;
    }
}