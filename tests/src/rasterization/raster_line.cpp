#include "test.h"
#include "graphics/rasterization/raster_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;
using namespace Catch;


SCENARIO("draw line", "[raster_line]")
{
    static std::mt19937_64 engine{Catch::getSeed()};
    const ivec2 p0 =
        GENERATE_REF(take(100, random_glm_vec<2, int, random_engine_wrapper<>>(-100, 100, engine)));
    const ivec2 p1 =
        GENERATE_REF(take(1, random_glm_vec<2, int, random_engine_wrapper<>>(-100, 100, engine)));
    ivec2 d =
        GENERATE_REF(take(1, random_glm_vec<2, int, random_engine_wrapper<>>(-100, 100, engine)));

    if(!conform_to(d.x <=> 0, p1.x - p0.x <=> 0)) d.x *= -1;
    if(!conform_to(d.y <=> 0, p1.y - p0.y <=> 0)) d.y *= -1;

    GIVEN(format("line p0 = {}, p1 = {}, d = {}", p0, p1, d))
    WHEN("draw it")
    {
        vector<ivec2> out1;
        vector<ivec2> out2;
        trivial_raster_line(p0, p1, back_inserter(out1), d);
        bresenham_raster_line(p0, p1, back_inserter(out2), d);

        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
    }
}