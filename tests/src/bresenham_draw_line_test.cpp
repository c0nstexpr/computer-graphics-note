#include "test.h"
#include "graphics/bresenham_draw_line.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics;

SCENARIO("draw test", "[bresenham_draw_line]")
{
    struct arg
    {
        ivec2 p0;
        decltype(p0) p1;
        decltype(p0)::value_type dx;

        vector<decltype(p0)> out;
    };

    const auto args = GENERATE(
        arg{
            {0, 0},
            {3, 3},
            1,
            {{0, 0}, {1, 1}, {2, 2}, {3, 3}} //
        },
        arg{
            {0, 0},
            {5, 2},
            1,
            {{0, 0}, {1, 0}, {2, 1}, {3, 1}, {4, 2}, {5, 2}} //
        },
        arg{
            {0, 0},
            {2, 5},
            1,
            {{0, 0}, {1, 3}, {2, 5}} //
        }
    );

    decltype(args.out) out{args.out.size()};

    bresenham_draw_line(args.p0, args.p1, out.begin(), args.dx, args.dx);

    REQUIRE_THAT(args.out, Catch::Matchers::RangeEquals(out));
}