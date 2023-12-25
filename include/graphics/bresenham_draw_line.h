#pragma once

#include <algorithm>
#include <stdsharp/cassert/cassert.h>
#include <stdsharp/utility/auto_cast.h>

#include "namespace_alias.h"

namespace graphics
{

    template<typename T = int, glm::qualifier Qualifier = glm::qualifier::defaultp>
    constexpr auto bresenham_draw_line(
        glm::vec<2, T, Qualifier> p0,
        const decltype(p0) p1,
        std::output_iterator<decltype(p1)> auto out_it,
        std::make_unsigned_t<T>& delta_x = 1,
        std::make_unsigned_t<T>& delta_y = 1,
    )
    {
        const auto line_a = p0.y - p1.y;
        const auto line_b = p1.x - p0.x;

    }
}