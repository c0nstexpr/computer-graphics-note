#include "test.h"

#include <graphics/rasterization/clip.h>

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;
using namespace Catch;

SCENARIO("Line clipping", "[clip]")
{
    static mt19937_64 engine{Catch::getSeed()};
    const auto plane_normal =
        GENERATE_REF(take(1000, random_glm_vec<3, float, random_engine_wrapper<>>(1, 10, engine)));
    const auto plane_p =
        GENERATE_REF(take(1, random_glm_vec<3, float, random_engine_wrapper<>>(0, 100, engine)));
    const mat2x3 line{
        GENERATE_REF(take(1, random_glm_vec<3, float, random_engine_wrapper<>>(0, 100, engine))),
        GENERATE_REF(take(1, random_glm_vec<3, float, random_engine_wrapper<>>(0, 100, engine)))
    };

    GIVEN( //
        format(
            "A plane as n={}, p={}, a line as a={}, b={}",
            plane_normal,
            plane_p,
            line[0],
            line[1]
        )
    )
    {
        const auto& out0 = line_clip(plane_normal, plane_p, line);
        const auto& out1 = line_clip(plane_normal, -dot(plane_normal, plane_p), line);

        REQUIRE(out0.first == out1.first);
        REQUIRE_THAT(out0.second, Matchers::WithinAbs(out1.second, 0.01));
    }
}
