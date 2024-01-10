#include "benchmark.h"
#include "graphics/rasterization/bresenham_draw_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

// SCENARIO("draw line", "[bresenham_draw_line]")
// {
//     const ivec2 p0 = GENERATE(take(10, Catch::random_glm_vec<2>(-5, 5)));
//     const ivec2 p1 = GENERATE(take(1, Catch::random_glm_vec<2>(-5, 5)));
//     auto dx = GENERATE(take(1, random(-5, 5)));
//     auto dy = GENERATE(take(1, random(-5, 5)));

//     if(!is_followed(dx <=> 0, p1.x - p0.x <=> 0)) dx *= -1;
//     if(!is_followed(dy <=> 0, p1.y - p0.y <=> 0)) dy *= -1;

//     GIVEN(format("line p0 = {}, p1 = {}, dx = {}, dy = {}", p0, p1, dx, dy))
//     WHEN("draw it")
//     {
//         vector<ivec2> out1, out2;
//         bresenham_draw_line(p0, p1, back_inserter(out2), dx, dy);

//         REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
//     }
// }