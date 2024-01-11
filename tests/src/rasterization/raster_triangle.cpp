#include "graphics/rasterization/raster_triangle.h"
#include "triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

SCENARIO("raster triangle", "[raster_triangle]") // NOLINT(*-cognitive-complexity)
{
    const auto [p0, p1, p2] = GENERATE(take(100, Catch::random_triangle(-100, 100)));

    GIVEN(format("triangle p0 = {}, p1 = {}, p2 = {}", p0, p1, p2))
    WHEN("rasterize it")
    {
        vector<ivec2> out1;
        vector<ivec2> out2;
        vector<ivec2> out3;
        vector<vec3> out_bcoor1;
        vector<vec3> out_bcoor2;
        vector<vec3> out_bcoor3;
        constexpr auto epsilon = 0.001f;

        trivial_raster_triangle<>(p0, p1, p2, back_inserter(out1), back_inserter(out_bcoor1));
        floating_incremental_raster_triangle<>(p0, p1, p2, back_inserter(out2), back_inserter(out_bcoor2));
        integral_incremental_raster_triangle<>(p0, p1, p2, back_inserter(out3), back_inserter(out_bcoor3));

        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out3));
        REQUIRE_THAT(out_bcoor1, Catch::Matchers::RangeEquals(out_bcoor3));

        REQUIRE(out_bcoor1.size() == out_bcoor2.size());

        for(const auto [b1, b2] : views::zip(out_bcoor1, out_bcoor2))
            for(auto i = 0; i < 3; ++i)
                REQUIRE_THAT(b1[i], Catch::Matchers::WithinAbs(b2[i], epsilon));
    }
}

static_assert((10 - (10.f / 3.f) * 3.0) > 0);