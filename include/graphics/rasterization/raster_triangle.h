#pragma once

#include <algorithm>

#include <stdsharp/concepts/concepts.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    template<typename OutT = float>
    struct trivial_raster_triangle_fn
    {
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
        constexpr void operator()(
            const glm::vec<2, T, Q>& p0,
            decltype(p0) p1,
            decltype(p0) p2,
            std::output_iterator<glm::vec<2, T, Q>> auto out_p,
            std::output_iterator<glm::vec<3, OutT, Q>> auto out_bcoor
        ) const
        {
            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = -f0_a * p0.x - f0_b * p0.y;

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = -f1_a * p1.x - f1_b * p1.y;

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = -f2_a * p2.x - f2_b * p2.y;

            const glm::vec<3, OutT, Q> f_den{
                f1_a * p0.x + f1_b * p0.y + f1_c,
                f2_a * p1.x + f2_b * p1.y + f2_c,
                f0_a * p2.x + f0_b * p2.y + f0_c
            };

            if(f_den[0] == 0) return;

            const glm::vec<3, OutT, Q> bc{f1_c, f2_c, f0_c};

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            for(glm::vec<2, T, Q> p{0, py.min}; p.y <= py.max; ++p.y)
            {
                const auto by =
                    glm::vec<3, OutT, Q>{f1_b, f2_b, f0_b} * static_cast<OutT>(p.y) + bc;

                for(p.x = px.min; p.x <= px.max; ++p.x)
                {
                    const auto bx =
                        (glm::vec<3, OutT, Q>{f1_a, f2_a, f0_a} * static_cast<OutT>(p.x) + by) /
                        f_den;

                    if(bx[0] <= 0 || bx[1] <= 0 || bx[2] <= 0) continue;

                    *out_p++ = p;
                    *out_bcoor++ = bx;
                }
            }
        }
    };

    template<typename OutT = float>
    inline constexpr trivial_raster_triangle_fn<OutT> trivial_raster_triangle{};

    template<typename OutT = float>
    struct fast_raster_triangle_fn
    {
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
        constexpr void operator()(
            const glm::vec<2, T, Q>& p0,
            decltype(p0) p1,
            decltype(p0) p2,
            std::output_iterator<glm::vec<2, T, Q>> auto out_p,
            std::output_iterator<glm::vec<3, OutT, Q>> auto out_bcoor
        ) const
        {
            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = -f0_a * p0.x - f0_b * p0.y;

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = -f1_a * p1.x - f1_b * p1.y;

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = -f2_a * p2.x - f2_b * p2.y;

            const glm::vec<3, OutT, Q> f_den{
                f1_a * p0.x + f1_b * p0.y + f1_c,
                f2_a * p1.x + f2_b * p1.y + f2_c,
                f0_a * p2.x + f0_b * p2.y + f0_c
            };

            if(f_den[0] == 0) return;

            const auto b_dx = glm::vec<3, OutT, Q>{f1_a, f2_a, f0_a} / f_den;
            const auto b_dy = glm::vec<3, OutT, Q>{f1_b, f2_b, f0_b} / f_den;

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            auto b_xy =
                glm::vec<3, OutT, Q>{
                    f1_a * px.min + f1_b * py.min + f1_c,
                    f2_a * px.min + f2_b * py.min + f2_c,
                    f0_a * px.min + f0_b * py.min + f0_c
                } /
                f_den;

            for(glm::vec<2, T, Q> p{0, py.min}; p.y <= py.max; ++p.y, b_xy += b_dy)
            {
                auto b_xy_tmp = b_xy;

                // triangle is convex
                for(p.x = px.min;
                    p.x <= px.max && (b_xy_tmp[0] <= 0 || b_xy_tmp[1] <= 0 || b_xy_tmp[2] <= 0);
                    ++p.x, b_xy_tmp += b_dx)
                {
                }

                for(; p.x <= px.max && b_xy_tmp[0] > 0 && b_xy_tmp[1] > 0 && b_xy_tmp[2] > 0;
                    ++p.x, b_xy_tmp += b_dx)
                {
                    *out_p++ = p;
                    *out_bcoor++ = b_xy_tmp;
                }
            }
        }
    };

    template<typename OutT = float>
    inline constexpr fast_raster_triangle_fn<OutT> fast_raster_triangle{};

    template<typename OutT = float>
    struct precise_raster_triangle_fn
    {
    private:
        static constexpr auto is_not_followed(const auto l, const auto r)
        {
            return is_eq(l) || l == r;
        }

    public:
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
        constexpr void operator()(
            const glm::vec<2, T, Q>& p0,
            decltype(p0) p1,
            decltype(p0) p2,
            std::output_iterator<glm::vec<2, T, Q>> auto out_p,
            std::output_iterator<glm::vec<3, OutT, Q>> auto out_bcoor
        ) const
        {
            // f0(x, y) => (y0 - y1)x + (x1 - x0)y + x0y1 - x1y0
            const auto f0_a = p0.y - p1.y; // y0 - y1
            const auto f0_b = p1.x - p0.x; // x1 - x0
            const auto f0_c = -f0_a * p0.x - f0_b * p0.y;

            // f1(x, y) => (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1
            const auto f1_a = p1.y - p2.y; // y1 - y2
            const auto f1_b = p2.x - p1.x; // x2 - x1
            const auto f1_c = -f1_a * p1.x - f1_b * p1.y;

            // f2(x, y) => (y2 - y0)x + (x0 - x2)y + x2y0 - x0y2
            const auto f2_a = p2.y - p0.y; // y2 - y0
            const auto f2_b = p0.x - p2.x; // x0 - x2
            const auto f2_c = -f2_a * p2.x - f2_b * p2.y;

            const glm::vec<3, OutT, Q> f_den{
                f1_a * p0.x + f1_b * p0.y + f1_c,
                f2_a * p1.x + f2_b * p1.y + f2_c,
                f0_a * p2.x + f0_b * p2.y + f0_c
            };

            if(f_den[0] == 0) return;

            const std::array f_den_cmp{f_den[0] <=> 0, f_den[1] <=> 0, f_den[2] <=> 0};

            const glm::vec<3, T, Q> f_dx{f1_a, f2_a, f0_a};
            const glm::vec<3, T, Q> f_dy{f1_b, f2_b, f0_b};

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            glm::vec<3, T, Q> f_xy{
                f1_a * px.min + f1_b * py.min + f1_c,
                f2_a * px.min + f2_b * py.min + f2_c,
                f0_a * px.min + f0_b * py.min + f0_c
            };

            for(glm::vec<2, T, Q> p{0, py.min}; p.y <= py.max; ++p.y, f_xy += f_dy)
            {
                auto out_f_xy = f_xy;

                // triangle is convex
                for(p.x = px.min; p.x <= px.max &&
                    ((out_f_xy[0] <=> 0) != f_den_cmp[0] || //
                     (out_f_xy[1] <=> 0) != f_den_cmp[1] || //
                     (out_f_xy[2] <=> 0) != f_den_cmp[2]);
                    ++p.x, out_f_xy += f_dx)
                {
                }

                for(; p.x <= px.max && //
                    (out_f_xy[0] <=> 0) == f_den_cmp[0] && //
                    (out_f_xy[1] <=> 0) == f_den_cmp[1] && //
                    (out_f_xy[2] <=> 0) == f_den_cmp[2];
                    ++p.x, out_f_xy += f_dx)
                {
                    *out_p++ = p;
                    *out_bcoor++ = glm::vec<3, OutT, Q>{
                        out_f_xy[0] / f_den[0],
                        out_f_xy[1] / f_den[1],
                        out_f_xy[2] / f_den[2]
                    };
                }
            }
        }
    };

    template<typename OutT = float>
    inline constexpr precise_raster_triangle_fn<OutT> precise_raster_triangle{};
}