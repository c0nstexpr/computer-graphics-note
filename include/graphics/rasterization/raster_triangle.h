#pragma once

#include <algorithm>
#include <functional>

#include <stdsharp/concepts/concepts.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    template<typename T, glm::qualifier Q = glm::defaultp>
    struct line_ab : glm::vec<2, T, Q>
    {
        using vec_t = glm::vec<2, T, Q>;
        using vec_t::vec_t;

        constexpr line_ab(const vec_t p0, const vec_t p1) noexcept: vec_t(p0.y - p1.y, p1.x - p0.x)
        {
        }

        [[nodiscard]] constexpr decltype(auto) a() const noexcept { return this->x; }

        [[nodiscard]] constexpr decltype(auto) b() const noexcept { return this->y; }
    };

    template<typename T, glm::qualifier Q = glm::defaultp>
    struct line_abc : glm::vec<3, T, Q>
    {
        using vec_t = glm::vec<3, T, Q>;
        using vec_t::vec_t;

        constexpr line_abc(const vec_t p0, const vec_t p1) noexcept:
            vec_t(p0.y - p1.y, p1.x - p0.x, p0.x * p1.y - p1.x * p0.y)
        {
        }

        [[nodiscard]] constexpr decltype(auto) a() const noexcept { return this->x; }

        [[nodiscard]] constexpr decltype(auto) b() const noexcept { return this->y; }

        [[nodiscard]] constexpr decltype(auto) c() const noexcept { return this->z; }
    };

    template<typename T, glm::qualifier Q = glm::defaultp>
    struct raster_triangle_default_predicate
    {
        using vec_t = glm::vec<2, T, Q>;

        using line_ab = line_ab<T, Q>;

        vec_t min{std::numeric_limits<T>::min()};
        vec_t max{std::numeric_limits<T>::max()};

        [[nodiscard]] constexpr bool
            operator()(const vec_t& current_p, const line_ab& line, const std::size_t /*unused*/)
                const noexcept
        {
            if(current_p.x == min.x || current_p.x == max.x || current_p.y == min.y ||
               current_p.y == max.y)
                return true;

            const auto cmp = line.x <=> 0;
            return is_lt(cmp) ? false : is_gt(cmp) || line.y > 0;
        }
    };

    template<glm::length_t L, typename OutT, typename T, glm::qualifier Q = glm::defaultp>
    struct barycentric_coordinate
    {
        using vec_t = glm::vec<L, T, Q>;
        using out_vec_t = glm::vec<L, OutT, Q>;

        vec_t point{};
        out_vec_t coordinate{};
    };
}

namespace graphics::rasterization::details
{
    template<typename OutT, typename Impl>
    struct raster_triangle_fn_base
    {
    private:
        template<typename T, glm::qualifier Q = glm::defaultp>
        static constexpr auto& reorder(std::array<glm::vec<2, T, Q>, 3>& p)
        {
            const auto u = p[1] - p[0];
            const auto v = p[2] - p[0];

            if(u.x * v.y < u.y * v.x) std::ranges::swap(p[0], p[1]);

            return p;
        }

        template<typename T, glm::qualifier Q = glm::defaultp>
        static constexpr auto get_den(
            const std::array<glm::vec<2, T, Q>, 3>& p,
            const std::array<line_abc<T, Q>, 3>& lines
        )
        {
            glm::vec<3, T, Q> res;

            for(std::size_t i = 0; i < 3; ++i)
            {
                const auto& line = lines[(i + 1) % 3];
                res[i] = line.a() * p[i].x + line.b() * p[i].y + line.c();
            }

            return res;
        }

    public:
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
            requires(!star::unsigned_<T>)
        constexpr void operator()(
            std::array<glm::vec<2, T, Q>, 3> p,
            std::output_iterator<barycentric_coordinate<3, OutT, T, Q>> auto out,
            std::predicate<
                const glm::vec<2, T, Q>&,
                const line_abc<T, Q>&,
                std::size_t> auto predicate = //
            [](const auto&...) { return false; }
        ) const
        {
            reorder(p);
            const std::array lines{{p[0], p[1]}, {p[1], p[2]}, {p[2], p[0]}};
            const auto& den = get_den(p, lines);

            // decayed triangle is line which might be drawn by other adjacent triangles
            // and boundary line should be handled by call side like predicate does
            if(den[0] == 0) return;

            const auto px = std::ranges::minmax({p[0].x, p[1].x, p[2].x});
            const auto py = std::ranges::minmax({p[0].y, p[1].y, p[2].y});
            Impl{}(out, predicate, lines, den, px, py);
        }
    };

    template<typename OutT>
    struct trivial_raster_triangle_fn
    {
        template<typename T, glm::qualifier Q>
        constexpr void operator()(
            auto& out,
            auto& predicate,
            const auto& lines,
            const auto& den,
            const auto& px,
            const auto& py
        ) const
        {
            using barycentric_coordinate = barycentric_coordinate<3, OutT, T, Q>;
            using out_vec_t = barycentric_coordinate::out_vec_t;

            const auto bc = [&]
            {
                out_vec_t res;

                for(std::size_t i = 0; i < 3; ++i)
                {
                    const auto& line = lines[(i + 1) % 3];
                    res[i] = line.c();
                }

                return res;
            }();

            const out_vec_t lines_a{lines[1].a(), lines[2].a(), lines[0].a()};
            const out_vec_t lines_b{lines[1].b(), lines[2].b(), lines[0].b()};

            for(glm::vec<2, T, Q> p{0, py.min}; p.y <= py.max; ++p.y)
            {
                const auto by = lines_b * static_cast<OutT>(p.y) + bc;

                for(p.x = px.min; p.x <= px.max; ++p.x)
                {
                    const auto bx = (lines_a * static_cast<OutT>(p.x) + by) / den;

                    if(bx.x < 0 || bx.y < 0 || bx.z < 0 || //
                       bx.x == 0 && !predicate(p, lines[1], 1) || //
                       bx.y == 0 && !predicate(p, lines[2], 2) || //
                       bx.z == 0 && !predicate(p, lines[0], 0))
                        continue;

                    *out++ = barycentric_coordinate{p, bx};
                }
            }
        }
    };

    template<typename OutT>
    struct floating_incremental_raster_triangle_fn
    {
        template<typename T, glm::qualifier Q>
        constexpr void operator()(
            auto& out,
            auto& predicate,
            const auto& lines,
            const auto& den,
            const auto& px,
            const auto& py
        ) const
        {
            using barycentric_coordinate = barycentric_coordinate<3, OutT, T, Q>;
            using out_vec_t = barycentric_coordinate::out_vec_t;
            using vec_t = barycentric_coordinate::vec_t;

            const std::array den_cmp{den[0] <=> 0, den[1] <=> 0, den[2] <=> 0};

            const vec_t f_dx{lines[1].a(), lines[2].a(), lines[0].a()};
            const vec_t f_dy{lines[1].b(), lines[2].b(), lines[0].b()};

            const auto b_dx = out_vec_t{f_dx} / den;
            const auto b_dy = out_vec_t{f_dy} / den;

            vec_t f_xy{
                lines[1].a() * px.min + lines[1].b() * py.min + lines[1].c(),
                lines[2].a() * px.min + lines[2].b() * py.min + lines[2].c(),
                lines[0].a() * px.min + lines[0].b() * py.min + lines[0].c()
            };

            // NOLINTBEGIN(cert-flp30-c)
            for(glm::vec<2, T, Q> p{0, py.min}; p.y <= py.max; ++p.y, f_xy += f_dy)
            {
                auto out_f_xy = f_xy;

                // triangle is convex
                for(p.x = px.min; p.x <= px.max &&
                    (out_f_xy[0] <=> 0 != den_cmp[0] || //
                     out_f_xy[1] <=> 0 != den_cmp[1] || //
                     out_f_xy[2] <=> 0 != den_cmp[2]);
                    ++p.x, out_f_xy += f_dx)
                {
                }

                for(auto b_xy = out_vec_t{out_f_xy} / den; p.x <= px.max && //
                    (out_f_xy[0] <=> 0) == den_cmp[0] && //
                    (out_f_xy[1] <=> 0) == den_cmp[1] && //
                    (out_f_xy[2] <=> 0) == den_cmp[2];
                    ++p.x, b_xy += b_dx, out_f_xy += f_dx)
                    *out++ = barycentric_coordinate{p, b_xy};

            } // NOLINTEND(cert-flp30-c)
        }

    public:
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
            requires(!star::unsigned_<T>)
        constexpr void operator()(
            std::array<glm::vec<2, T, Q>, 3> p,
            std::output_iterator<barycentric_coordinate<3, OutT, T, Q>> auto out,
            std::predicate<
                const glm::vec<2, T, Q>&,
                const line_abc<T, Q>&,
                std::size_t> auto predicate = //
            [](const auto&...) { return false; }
        )
    };
}

namespace graphics::rasterization
{
    template<typename OutT = float>
    using trivial_raster_triangle_fn =
        details::raster_triangle_fn_base<OutT, details::trivial_raster_triangle_fn<OutT>>;

    template<typename OutT = float>
    inline constexpr trivial_raster_triangle_fn<OutT> trivial_raster_triangle{};


    template<typename OutT = float>
    inline constexpr floating_incremental_raster_triangle_fn<OutT>
        floating_incremental_raster_triangle{};

    template<typename OutT = float>
    struct integral_incremental_raster_triangle_fn
    {
        template<star::explicitly_convertible<OutT> T, glm::qualifier Q>
            requires(!star::unsigned_<T>)
        constexpr void operator()(
            const glm::vec<2, T, Q>& p0,
            decltype(p0) p1,
            decltype(p0) p2,
            std::output_iterator<glm::vec<2, T, Q>> auto out_p,
            std::output_iterator<glm::vec<3, OutT, Q>> auto out_bcoor
        ) const
        {
            using t_vec = glm::vec<3, T, Q>;
            using out_t_vec = glm::vec<3, OutT, Q>;

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

            const out_t_vec f_den{
                f1_a * p0.x + f1_b * p0.y + f1_c,
                f2_a * p1.x + f2_b * p1.y + f2_c,
                f0_a * p2.x + f0_b * p2.y + f0_c
            };

            if(f_den[0] == 0) return;

            const std::array f_den_cmp{f_den[0] <=> 0, f_den[1] <=> 0, f_den[2] <=> 0};

            const t_vec f_dx{f1_a, f2_a, f0_a};
            const t_vec f_dy{f1_b, f2_b, f0_b};

            const auto px = std::ranges::minmax({p0.x, p1.x, p2.x});
            const auto py = std::ranges::minmax({p0.y, p1.y, p2.y});

            t_vec f_xy{
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
                    *out_bcoor++ = out_t_vec{out_f_xy} / f_den;
                }
            }
        }
    };

    template<typename OutT = float>
    inline constexpr integral_incremental_raster_triangle_fn<OutT>
        integral_incremental_raster_triangle{};
}