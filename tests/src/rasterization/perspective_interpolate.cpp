#include <glm/gtc/matrix_transform.hpp>

#include "test.h"

#include "graphics/rasterization/perspective_interpolate.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;
using namespace Catch;

SCENARIO("perspective interpolate test", "[perspective_interpolate]")
{
    static std::mt19937_64 engine{Catch::getSeed()};
    const auto a =
        GENERATE_REF(take(100, random_glm_vec<1, float, random_engine_wrapper<>>(0, 1, engine))).x;
    const auto b =
        GENERATE_REF(take(1, random_glm_vec<1, float, random_engine_wrapper<>>(0, 1 - a, engine)))
            .x;
    const auto c = 1 - a - b;

    GIVEN(format("barycentric coordinate a = {}, b = {}", a, b))
    {
        std::ranges::single_view<vec3> out0;
        std::ranges::single_view<vec3> out1;
        {
            const array<vec3, 3> triangle{vec3{30, -30, -40}, {-30, -30, -50}, {0, 30, -60}};
            const auto p = a * triangle[0] + b * triangle[1] + c * triangle[2];
            const auto mat = perspective(radians(90.f), 1.f, 1.f, 100.f);
            const array<vec4, 3> projected_triangle{
                mat * vec4{triangle[0], 1},
                mat * vec4{triangle[1], 1},
                mat * vec4{triangle[2], 1}
            };
            const auto& barycentric = std::views::single(vec3{a, b, c});

            trivial_perspective_interpolate(projected_triangle, mat, barycentric, out0.begin());
            perspective_interpolate(
                vec3{projected_triangle[0].w, projected_triangle[1].w, projected_triangle[2].w},
                barycentric,
                out1.begin()
            );
        }

        constexpr auto error = .001;

        REQUIRE_THAT(::ranges::accumulate(out0.front(), 0.f), Matchers::WithinAbs(1, error));
        REQUIRE_THAT(::ranges::accumulate(out1.front(), 0.f), Matchers::WithinAbs(1, error));
        REQUIRE_THAT(out0.front().x, Matchers::WithinAbs(out1.front().x, error));
        REQUIRE_THAT(out0.front().y, Matchers::WithinAbs(out1.front().y, error));
        REQUIRE_THAT(out0.front().z, Matchers::WithinAbs(out1.front().z, error));
    }
}