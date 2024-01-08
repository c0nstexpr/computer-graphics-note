#include "test.h"
#include "graphics/rasterization/raster_triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

SCENARIO("raster triangle", "[raster_triangle]")
{
    const ivec2 p0 = GENERATE(take(10, Catch::random_glm_vec<2>(-5, 5)));
    const ivec2 p1 = GENERATE(take(1, Catch::random_glm_vec<2>(-5, 5)));
    const ivec2 p2 = GENERATE(take(1, Catch::random_glm_vec<2>(-5, 5)));

    GIVEN(format("triangle p0 = {}, p1 = {}, p2 = {}", p0, p1, p2))
    WHEN("rasterize it")
    {
        vector<ivec2> out1, out2;
        vector<vec2> out_bcoor1, out_bcoor2;
        out1.clear();
        out2.clear();
        out_bcoor1.clear();
        out_bcoor2.clear();

        trivial_raster_triangle<>(p0, p1, p2, back_inserter(out1), back_inserter(out_bcoor1));
        raster_triangle<>(p0, p1, p2, back_inserter(out2), back_inserter(out_bcoor2));

        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
        REQUIRE_THAT(out_bcoor1, Catch::Matchers::RangeEquals(out_bcoor2));
    }
}