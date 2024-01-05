#include "test.h"
#include "graphics/bresenham_draw_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics;

bool is_followed(const strong_ordering l, const strong_ordering r)
{
    return is_eq(l) || (l == r);
}

auto validate_vec(ivec2 p0, decltype(p0) p1, decltype(p0)::value_type dx)
{
    const auto x_diff = p1.x - p0.x;
    const auto y_diff = p1.y - p0.y;
    const auto x_diff_cmp = x_diff <=> 0;
    const auto y_diff_cmp = y_diff <=> 0;

    const auto line_a = y_diff;
    const auto line_b = -x_diff;

    vector<decltype(p0)> out;

    if(x_diff == 0)
    {
        for(; is_followed(p1.y <=> p0.y, y_diff_cmp); p0.y += dx)
            out.emplace_back(p0);
         return ;
    }

    const auto line_c = -p0.x * line_a - p0.y * line_b;
    size_t i = 0;
    const auto x = p0.x + dx * i;
    const auto y = static_cast<long double>(line_a * x + line_c) / -line_b;
    const auto y_step = std::round((y - p0.y) / dx);

    return {x, p0.y + dx * y_step};
}

SCENARIO("draw line", "[bresenham_draw_line]")
{
    struct arg
    {
        ivec2 p0;
        decltype(p0) p1;
        decltype(p0)::value_type dx;

        vector<decltype(p0)> out;

        validate_iterator validator() const { return {{}, p0, p1, dx}; }
    };

    const auto args = GENERATE(
        arg{{0, 0}, {3, 3}, 0},
        arg{
            {0, 0},
            {5, 2},
            0,
            {{0, 0}, {1, 0}, {2, 1}, {3, 1}, {4, 2}, {5, 2}} //
        },
        arg{
            {0, 0},
            {2, 5},
            0,
            {{0, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 4}, {2, 5}} //
        }
    );


    bresenham_draw_line(args.p0, args.p1, out.begin(), args.dx, args.dx);

    REQUIRE_THAT(args.out, Catch::Matchers::RangeEquals(out));
}