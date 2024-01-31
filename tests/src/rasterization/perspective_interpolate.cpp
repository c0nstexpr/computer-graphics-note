#include <glm/ext/matrix_clip_space.hpp>

#include "test.h"

#include "graphics/rasterization/perspective_interpolate.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;
using namespace Catch;

SCENARIO("perspective interpolate test", "[perspective_interpolate]")
{
    static mt19937_64 engine{Catch::getSeed()};
    const auto a =
        GENERATE_REF(take(1000, random_glm_vec<1, float, random_engine_wrapper<>>(0, 1, engine))).x;
    const auto b =
        GENERATE_REF(take(1, random_glm_vec<1, float, random_engine_wrapper<>>(0, 1 - a, engine)))
            .x;
    const auto c = 1 - a - b;
    const auto aspect =
        GENERATE_REF(take(1, random_glm_vec<1, float, random_engine_wrapper<>>(0, 100, engine))).x;
    const auto& barycentric = views::single(vec3{a, b, c});

    GIVEN(format("barycentric coordinate a = {}, b = {}, c = {}", a, b, c))
    {
        constexpr auto error = .001;
        auto out0 = views::single(vec3{});
        auto out1 = views::single(vec3{});
        auto out2 = views::single(vec3{});

        {
            constexpr mat3x3 triangle{vec3{30, -30, -40}, {-30, -30, -50}, {0, 30, -60}};
            const auto mat = perspective(radians(90.f), aspect, 1.f, 100.f);
            const mat3x4 projected_triangle{
                mat * vec4{triangle[0], 1},
                mat * vec4{triangle[1], 1},
                mat * vec4{triangle[2], 1}
            };
            const vec3 depths{
                projected_triangle[0].w,
                projected_triangle[1].w,
                projected_triangle[2].w
            };

            trivial_perspective_interpolate(projected_triangle, mat, barycentric, out0.begin());
            depth_mul_perspective_interpolate(depths, barycentric, out1.begin());
            depth_div_perspective_interpolate(depths, barycentric, out2.begin());
        }

        REQUIRE_THAT(::ranges::accumulate(out0.front(), 0.f), Matchers::WithinAbs(1, error));
        REQUIRE_THAT(::ranges::accumulate(out1.front(), 0.f), Matchers::WithinAbs(1, error));
        REQUIRE_THAT(::ranges::accumulate(out2.front(), 0.f), Matchers::WithinAbs(1, error));
        REQUIRE_THAT(out0.front().x, Matchers::WithinAbs(out1.front().x, error));
        REQUIRE_THAT(out0.front().y, Matchers::WithinAbs(out1.front().y, error));
        REQUIRE_THAT(out0.front().z, Matchers::WithinAbs(out1.front().z, error));
        REQUIRE_THAT(out0.front().x, Matchers::WithinAbs(out2.front().x, error));
        REQUIRE_THAT(out0.front().y, Matchers::WithinAbs(out2.front().y, error));
        REQUIRE_THAT(out0.front().z, Matchers::WithinAbs(out2.front().z, error));
    }
}