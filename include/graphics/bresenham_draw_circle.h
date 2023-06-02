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
        constexpr OutIt symmetric_circle_draw(const glm::vec<2, T, Qualifier> p, OutIt out_it)
        {
            *out_it++ = p;
            *out_it++ = {-p.x, p.y};
            *out_it++ = {p.x, -p.y};
            *out_it++ = {-p.x, -p.y};

            *out_it++ = {p.y, p.x};
            *out_it++ = {-p.y, p.x};
            *out_it++ = {p.y, -p.x};
            *out_it++ = {-p.y, -p.x};

            return out_it;
        }
    }

    template<
        typename T,
        glm::qualifier Qualifier = glm::qualifier::defaultp,
        std::output_iterator<glm::vec<2, T, Qualifier>> OutIt // clang-format off
    > // clang-format on
        requires std::is_signed_v<T>
    constexpr OutIt bresenham_draw_circle(
        const glm::vec<2, T, Qualifier> center,
        const std::make_unsigned_t<T> radius,
        const std::make_unsigned_t<T> dx = 1,
        OutIt out_it,
    )
    {
        return out_it;
    }
}