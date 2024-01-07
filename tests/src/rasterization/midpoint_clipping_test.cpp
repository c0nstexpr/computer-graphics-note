#include <glm/gtc/epsilon.hpp>

#include "test.h"
#include "graphics/rasterization/midpoint_clipping.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

SCENARIO("midpoint clip test", "[midpoint_clipping]")
{
    struct points
    {
        ivec2 p0;
        ivec2 p1;

        dvec2 out_p0;
        dvec2 out_p1;
    };

    const auto [in_p0, in_p1, out_p0, out_p1] = GENERATE(
        points{{0, 0}, {10, 10}, {0, 0}, {10, 10}},
        points{{2, 3}, {5, 5}, {2, 3}, {5, 5}},
        points{{-15, 0}, {15, 12}, {0, 6}, {10, 10}},
        points{{-5, 5}, {5, -3}, {0, 1}, {1.25, 0}},
        points{{-5, 5}, {5, 3}, {0, 4}, {5, 3}}
    );

    const auto [p0, p1] = midpoint_clipping(in_p0, in_p1, {}, {10, 10});

    constexpr auto error = 0.001;

    REQUIRE(all(epsilonEqual(p0, out_p0, error)));
    REQUIRE(all(epsilonEqual(p1, out_p1, error)));
}