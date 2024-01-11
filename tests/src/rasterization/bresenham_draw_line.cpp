#include "test.h"
#include "graphics/rasterization/bresenham_draw_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

bool is_followed(const auto l, const auto r) { return is_eq(l) || (l == r); }

void validate_fn(
    const ivec2 p0,
    const ivec2 p1,
    auto it,
    ivec2::value_type dx,
    ivec2::value_type dy
)
{
    const auto diff = p1 - p0;
    const auto x_diff_cmp = diff.x <=> 0;
    const auto y_diff_cmp = diff.y <=> 0;

    const auto line_a = diff.y;
    const auto line_b = -diff.x;
    const auto line_c = -p0.x * line_a - p0.y * line_b;

    if(dx == 0) dx = diff.x > 0 ? 1 : -1;
    if(dy == 0) dy = diff.y > 0 ? 1 : -1;

    const auto half_dy = dy / 2.0;

    if(diff.x == 0)
    {
        for(auto y = p0.y; is_followed(p1.y <=> y, y_diff_cmp); y += dy) *(it++) = ivec2{p0.x, y};
        return;
    }

    for(auto x = p0.x, y = p0.y; is_followed(p1.x <=> x, x_diff_cmp); x += dx)
    {
        const auto exact_y = static_cast<long double>(line_a * x + line_c) / -line_b;

        if(!is_followed(exact_y <=> (y + half_dy), y_diff_cmp)) *(it++) = ivec2{x, y};
        else
            while(true)
            {
                y += dy;

                if(!is_followed(p1.y <=> y, y_diff_cmp)) return;

                *(it++) = ivec2{x, y};

                if(!is_followed(exact_y <=> (y + half_dy), y_diff_cmp)) break;
            }
    }
}

SCENARIO("draw line", "[bresenham_draw_line]")
{
    const ivec2 p0 = GENERATE(take(100, Catch::random_glm_vec<2>(-100, 100)));
    const ivec2 p1 = GENERATE(take(1, Catch::random_glm_vec<2>(-100, 100)));
    auto dx = GENERATE(take(1, random(-100, 100)));
    auto dy = GENERATE(take(1, random(-100, 100)));

    if(!is_followed(dx <=> 0, p1.x - p0.x <=> 0)) dx *= -1;
    if(!is_followed(dy <=> 0, p1.y - p0.y <=> 0)) dy *= -1;

    GIVEN(format("line p0 = {}, p1 = {}, dx = {}, dy = {}", p0, p1, dx, dy))
    WHEN("draw it")
    {
        vector<ivec2> out1;
        vector<ivec2> out2;
        validate_fn(p0, p1, back_inserter(out1), dx, dy);
        bresenham_draw_line(p0, p1, back_inserter(out2), dx, dy);

        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
    }
}