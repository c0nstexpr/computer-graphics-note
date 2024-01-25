#pragma once

#include <algorithm>

#include <array>
#include <compare>
#include <gsl/assert>
#include <stdsharp/concepts/concepts.h>
#include <stdsharp/cassert/cassert.h>

#include "../namespace_alias.h"

namespace graphics::rasterization
{
    template<typename T, glm::qualifier Q = glm::defaultp>
    struct line_ab : glm::vec<2, T, Q>
    {
        using vec_t = glm::vec<2, T, Q>;
        using vec_t::vec_t;

        constexpr line_ab(const glm::vec<2, T, Q> p0, const glm::vec<2, T, Q> p1) noexcept:
            vec_t(p0.y - p1.y, p1.x - p0.x)
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

        constexpr line_abc(const glm::vec<2, T, Q> p0, const glm::vec<2, T, Q> p1) noexcept:
            vec_t(p0.y - p1.y, p1.x - p0.x, p0.x * p1.y - p1.x * p0.y)
        {
        }

        [[nodiscard]] constexpr decltype(auto) a() const noexcept { return this->x; }

        [[nodiscard]] constexpr decltype(auto) b() const noexcept { return this->y; }

        [[nodiscard]] constexpr decltype(auto) c() const noexcept { return this->z; }
    };

    template<typename T, glm::qualifier Q = glm::defaultp>
    struct raster_triangle_windowed_predicate
    {
        using vec_t = glm::vec<2, T, Q>;

        using line_abc = line_abc<T, Q>;

        vec_t min{std::numeric_limits<T>::min()};
        vec_t max{std::numeric_limits<T>::max()};

        [[nodiscard]] constexpr bool
            operator()(const vec_t& current_p, const line_abc& line, const std::size_t /*unused*/)
                const noexcept
        {
            if(current_p.x == min.x || current_p.x == max.x || current_p.y == min.y ||
               current_p.y == max.y)
                return true;

            const auto cmp = line.a() <=> 0;
            return is_lt(cmp) ? false : is_gt(cmp) || line.b() > 0;
        }
    };

    template<glm::length_t L, typename OutT, typename T, glm::qualifier Q = glm::defaultp>
    struct barycentric_coordinate
    {
        using vec_t = glm::vec<L, T, Q>;
        using out_vec_t = glm::vec<3, OutT, Q>;

        vec_t point{};
        out_vec_t coordinate{};

        friend bool
            operator==(const barycentric_coordinate&, const barycentric_coordinate&) = default;
    };
}

namespace graphics::rasterization::details
{
    template<typename OutT, template<typename> typename Impl>
    struct raster_triangle_fn_base
    {
    private:
        static constexpr auto default_predicate = [](const auto&...) { return true; };

    public:
        template<
            star::explicitly_convertible<OutT> T,
            glm::qualifier Q = glm::defaultp,
            std::predicate<const glm::vec<2, T, Q>&, const line_abc<T, Q>&, const std::size_t&>
                Pred = decltype(default_predicate)>
            requires(!star::unsigned_<T>)
        constexpr void operator()(
            std::array<glm::vec<2, T, Q>, 3> p,
            std::output_iterator<barycentric_coordinate<2, OutT, T, Q>> auto out,
            Pred predicate = default_predicate
        ) const
        {
            using vec3_t = glm::vec<3, T, Q>;

            {
                const auto u = p[1] - p[0];
                const auto v = p[2] - p[1];

                Expects(u.x * v.y > u.y * v.x);
            }

            const auto& lines =
                std::to_array<line_abc<T, Q>>({{p[0], p[1]}, {p[1], p[2]}, {p[2], p[0]}});
            const vec3_t den{
                lines[1].a() * p[0].x + lines[1].b() * p[0].y + lines[1].c(),
                lines[2].a() * p[1].x + lines[2].b() * p[1].y + lines[2].c(),
                lines[0].a() * p[2].x + lines[0].b() * p[2].y + lines[0].c()
            };

            // decayed triangle is line which might be drawn by other adjacent triangles
            // and boundary line should be handled by call side like predicate does
            if(den[0] == 0) return;

            Impl<OutT>{}( //
                out,
                predicate,
                lines,
                den,
                std::ranges::minmax({p[0].x, p[1].x, p[2].x}),
                std::ranges::minmax({p[0].y, p[1].y, p[2].y})
            );
        }
    };

    template<typename OutT>
    struct trivial_raster_triangle_fn
    {
    private:
        static constexpr bool is_valid(
            const OutT& b,
            auto& predicate,
            const auto& p,
            const auto& line,
            const auto& i
        )
        {
            const auto cmp = b <=> 0;
            return is_lt(cmp) ? false : is_gt(cmp) || predicate(p, line, i);
        }

    public:
        template<typename T, glm::qualifier Q>
        constexpr void operator()(
            auto& out,
            auto& predicate,
            const std::array<line_abc<T, Q>, 3>& lines,
            const auto& den,
            const auto& px,
            const auto& py
        ) const
        {
            using barycentric_coordinate = barycentric_coordinate<2, OutT, T, Q>;
            using vec_t = barycentric_coordinate::vec_t;
            using out_vec_t = barycentric_coordinate::out_vec_t;
            using vec3_t = glm::vec<3, T, Q>;

            const out_vec_t f_a{lines[1].a(), lines[2].a(), lines[0].a()};
            const out_vec_t f_b{lines[1].b(), lines[2].b(), lines[0].b()};
            const out_vec_t f_c{lines[1].c(), lines[2].c(), lines[0].c()};
            const out_vec_t out_den{den};

            for(vec_t p{0, py.min}; p.y <= py.max; ++p.y)
            {
                const auto fxy = f_b * static_cast<OutT>(p.y) + f_c;

                for(p.x = px.min; p.x <= px.max; ++p.x)
                    if(const auto out_fxy = (f_a * static_cast<OutT>(p.x) + fxy) / out_den;
                       is_valid(out_fxy[0], predicate, p, lines[1], 0) &&
                       is_valid(out_fxy[1], predicate, p, lines[2], 1) &&
                       is_valid(out_fxy[2], predicate, p, lines[0], 2))
                        *out++ = barycentric_coordinate{p, out_fxy};
            }
        }
    };

    template<typename OutT = float>
    struct integral_incremental_raster_triangle_fn
    {
    private:
        static constexpr auto is_valid_impl(
            const auto& f,
            const auto& den_cmp,
            auto& predicate,
            const auto& p,
            const auto& line,
            const auto& i
        )
        {
            const auto cmp = f <=> 0;
            return den_cmp == cmp || is_eq(cmp) && predicate(p, line, i);
        };

    public:
        static constexpr auto is_valid = []( //
                                             const auto& out_f_xy,
                                             const auto& den_cmp,
                                             auto& predicate,
                                             const auto& p,
                                             const auto& lines
                                         )
        {
            return is_valid_impl(out_f_xy[0], den_cmp[0], predicate, p, lines[1], 0) &&
                is_valid_impl(out_f_xy[1], den_cmp[1], predicate, p, lines[2], 1) &&
                is_valid_impl(out_f_xy[2], den_cmp[2], predicate, p, lines[0], 2);
        };

        template<typename T, glm::qualifier Q>
        constexpr void operator()(
            auto& out,
            auto& predicate,
            const std::array<line_abc<T, Q>, 3>& lines,
            const auto& den,
            const auto& px,
            const auto& py
        ) const
        {
            using barycentric_coordinate = barycentric_coordinate<2, OutT, T, Q>;
            using out_vec_t = barycentric_coordinate::out_vec_t;
            using vec_t = barycentric_coordinate::vec_t;
            using vec3_t = glm::vec<3, T, Q>;

            const std::array den_cmp{den[0] <=> 0, den[1] <=> 0, den[2] <=> 0};
            const vec3_t f_dx{lines[1].a(), lines[2].a(), lines[0].a()};
            const vec3_t f_dy{lines[1].b(), lines[2].b(), lines[0].b()};
            vec3_t f_xy{lines[1].c(), lines[2].c(), lines[0].c()};
            const out_vec_t out_den{den};

            f_xy += px.min * f_dx + py.min * f_dy;

            for(vec_t p{0, py.min}; p.y <= py.max; ++p.y, f_xy += f_dy)
            {
                auto out_f_xy = f_xy;

                // triangle is convex
                for(p.x = px.min;
                    p.x <= px.max && !is_valid(out_f_xy, den_cmp, predicate, p, lines);
                    ++p.x, out_f_xy += f_dx)
                {
                }

                for(; p.x <= px.max && is_valid(out_f_xy, den_cmp, predicate, p, lines);
                    ++p.x, out_f_xy += f_dx)
                    *out++ = barycentric_coordinate{p, out_vec_t{out_f_xy} / out_den};
            }
        }
    };
}

namespace graphics::rasterization
{
    template<typename OutT = float>
    using trivial_raster_triangle_fn =
        details::raster_triangle_fn_base<OutT, details::trivial_raster_triangle_fn>;

    template<typename OutT = float>
    inline constexpr trivial_raster_triangle_fn<OutT> trivial_raster_triangle{};

    template<typename OutT = float>
    using integral_incremental_raster_triangle_fn =
        details::raster_triangle_fn_base<OutT, details::integral_incremental_raster_triangle_fn>;

    template<typename OutT = float>
    inline constexpr integral_incremental_raster_triangle_fn<OutT>
        integral_incremental_raster_triangle{};
}