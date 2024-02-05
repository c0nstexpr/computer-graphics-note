#pragma once

#include <range/v3/range/operations.hpp>
#include <ranges>
#include <stdsharp/concepts/concepts.h>

#include "../namespace_alias.h"

namespace graphics::texture_mapping
{
    inline constexpr struct bilinear_filtering_fn
    {
        template<
            typename T,
            glm::qualifier Q = glm::defaultp,
            std::ranges::random_access_range TextureRng>
            requires requires(std::ranges::range_reference_t<TextureRng> texture_rng) {
                requires std::numeric_limits<T>::is_iec559;
                requires std::ranges::random_access_range<decltype(texture_rng)>;
                requires star::arithmetic_like<
                    std::ranges::range_reference_t<decltype(texture_rng)>>;
                requires star::explicitly_convertible<
                    T,
                    std::ranges::range_difference_t<decltype(texture_rng)>>;
            }
        constexpr auto operator()(const glm::vec<2, T, Q>& uv, TextureRng&& rng) const
        {
            const auto& uv_floor = glm::floor(uv);
            const auto& uv_next = uv_floor + T{1};
            const auto& a = uv_next - uv;
            const auto& b = uv - uv_floor;

            ::ranges::index;

            using diff_t = std::ranges::range_difference_t<decltype(rng)>;

            const auto& u_floor_iter =
                std::ranges::cbegin(std::ranges::cbegin(rng)[static_cast<diff_t>(uv_floor.x)]);
            const auto& u_next_iter =
                std::ranges::cbegin(std::ranges::cbegin(rng)[static_cast<diff_t>(uv_next.x)]);

            using iter_diff_t = std::iter_difference_t<std::decay_t<decltype(u_floor_iter)>>;

            const auto& v_floor = static_cast<iter_diff_t>(uv_floor.y);
            const auto& v_next = static_cast<iter_diff_t>(uv_next.y);

            return a.y * (a.x * u_floor_iter[v_floor] + b.x * u_next_iter[v_floor]) +
                b.y * (a.x * u_floor_iter[v_next] + b.x * u_next_iter[v_next]);
        }
    } bilinear_filtering{};

    inline constexpr struct trilinear_filtering_fn
    {
        template<
            typename T,
            glm::qualifier Q = glm::defaultp,
            std::ranges::random_access_range MinmapRng>
            requires requires(std::ranges::range_reference_t<MinmapRng> texture_rng) {
                requires std::numeric_limits<T>::is_iec559;
                requires std::ranges::random_access_range<decltype(texture_urng)>;
                requires star::arithmetic_like<
                    std::ranges::range_reference_t<decltype(texture_urng)>>;
                requires star::explicitly_convertible<
                    T,
                    std::ranges::range_difference_t<decltype(texture_urng)>>;
            }
        constexpr auto operator()(
            const glm::vec<2, T, Q>& uv,
            const std::type_identity_t<T>& minmap_level,
            MinmapRng&& minmaps,
            std::input_iterator auto minmaps_interpolate_it
        ) const
        {
        }
    } trilinear_filtering{};

    inline constexpr struct nearest_filtering_fn
    {
        template<
            typename T,
            glm::qualifier Q = glm::defaultp,
            std::invocable<const glm::vec<2, T, Q>&> GetTextureFn>
            requires star::arithmetic_like<
                         std::invoke_result_t<GetTextureFn, const glm::vec<2, T, Q>&>> &&
            std::numeric_limits<T>::is_iec559
        constexpr auto operator()(const glm::vec<2, T, Q>& uv, GetTextureFn fn) const
        {
            return std::invoke(fn, glm::round(uv));
        }
    } nearest_filtering{};
}