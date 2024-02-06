#pragma once

#include <range/v3/range/operations.hpp>
#include <stdsharp/ranges/ranges.h>

#include "../traits.h"

namespace stdsharp
{
#define STDSHARP_MD_RANGE_TRAITS(name, ns)                             \
    template<std::size_t N, typename T>                                \
    struct md_##name                                                   \
    {                                                                  \
        using rng = T;                                                 \
                                                                       \
        using type = ns::name##_t<typename md_##name<N - 1, T>::type>; \
    };                                                                 \
                                                                       \
    template<typename T>                                               \
    struct md_##name<0, T>                                             \
    {                                                                  \
        using rng = T;                                                 \
                                                                       \
        using type = ns::name##_t<T>;                                  \
    };                                                                 \
                                                                       \
    template<std::size_t N, typename T>                                \
    using md_##name##_t = typename md_##name<N, T>::type;

    STDSHARP_MD_RANGE_TRAITS(iterator, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(const_iterator, stdsharp)
    STDSHARP_MD_RANGE_TRAITS(sentinel, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(const_sentinel, stdsharp)
    STDSHARP_MD_RANGE_TRAITS(range_size, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(range_difference, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(range_value, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(range_reference, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(range_const_reference, stdsharp)
    STDSHARP_MD_RANGE_TRAITS(range_rvalue_reference, std::ranges)
    STDSHARP_MD_RANGE_TRAITS(range_common_reference, std::ranges)

#undef STDSHARP_MD_RANGE_TRAITS
}

namespace graphics::texture_mapping
{
    inline constexpr struct bilinear_filtering_fn
    {
        template<
            std::ranges::random_access_range TextureRng,
            star::explicitly_convertible<std::ranges::range_difference_t<TextureRng>> T = float,
            glm::qualifier Q = glm::defaultp>
            requires requires {
                requires std::floating_point<T>;
                requires std::ranges::random_access_range<
                    std::ranges::range_reference_t<TextureRng>>;
                requires star::
                    explicitly_convertible<T, star::md_range_difference_t<0, TextureRng>>;
                glm_vec_traits<star::md_range_reference_t<1, T>>{};
            }
        constexpr auto operator()(const glm::vec<2, T, Q>& uv, TextureRng&& rng) const
        {
            using ::ranges::index;
            using star::auto_cast;

            const auto& uv_floor = glm::floor(uv);
            const auto& uv_next = uv_floor + T{1};
            const auto& a = uv_next - uv;
            const auto& b = uv - uv_floor;

            using u_diff_t = std::ranges::range_difference_t<decltype(rng)>;

            const auto& u_floor = index(rng, static_cast<u_diff_t>(uv_floor.x));
            const auto& u_next = index(rng, static_cast<u_diff_t>(uv_next.x));

            using v_diff_t = star::md_range_difference_t<0, TextureRng>;

            const v_diff_t v_floor = auto_cast(uv_floor.y);
            const v_diff_t v_next = auto_cast(uv_next.y);

            return a.y * (a.x * index(u_floor, v_floor) + b.x * index(u_next, v_floor)) +
                b.y * (a.x * index(u_floor, v_next) + b.x * index(u_next, v_next));
        }
    } bilinear_filtering{};

    inline constexpr struct trilinear_filtering_fn
    {
        template<
            std::ranges::random_access_range MinmapRng,
            star::explicitly_convertible<std::ranges::range_difference_t<MinmapRng>> U = float>
        constexpr auto operator()(const auto& uv, const U& minmap_level, MinmapRng&& minmaps) const
            requires requires {
                requires std::floating_point<U>;
                requires std::invocable<
                    bilinear_filtering_fn,
                    decltype(uv),
                    std::ranges::range_reference_t<MinmapRng>>;
            }
        {
            using ::ranges::index;
            using diff_t = std::ranges::range_difference_t<MinmapRng>;

            const auto& minmap_level_floor = std::floor(minmap_level);
            const auto& minmap_level_next = minmap_level_floor + 1;

            return (minmap_level_next - minmap_level) *
                bilinear_filtering(uv, index(minmaps, static_cast<diff_t>(minmap_level_floor))) +
                (minmap_level - minmap_level_floor) *
                bilinear_filtering(uv, index(minmaps, static_cast<diff_t>(minmap_level_next)));
        }
    } trilinear_filtering{};

    inline constexpr struct nearest_filtering_fn
    {
        template<
            std::ranges::random_access_range TextureRng,
            star::explicitly_convertible<std::ranges::range_difference_t<TextureRng>> T = float,
            glm::qualifier Q = glm::defaultp>
            requires requires {
                requires std::floating_point<T>;
                requires std::ranges::random_access_range<
                    std::ranges::range_reference_t<TextureRng>>;
                requires star::
                    explicitly_convertible<T, star::md_range_difference_t<1, TextureRng>>;
            }
        constexpr auto operator()(glm::vec<2, T, Q> uv, TextureRng&& rng) const
        {
            using ::ranges::index;

            uv = glm::round(uv);

            return index(index(rng, static_cast<std::ranges::range_difference_t<TextureRng>>(uv.x)), static_cast<star::md_range_difference_t<1, TextureRng>>(uv.y));
        }
    } nearest_filtering{};
}