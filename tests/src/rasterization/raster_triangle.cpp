#include "graphics/rasterization/raster_triangle.h"
#include "triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

void validate(const auto p0, const auto p1, const auto p2)
{
    validate(p0, p1, p2, [](const auto&...) { return true; });
}

void validate(const auto p0, const auto p1, const auto p2, const auto predicate)
{
    vector<barycentric_coordinate<2, float, int>> out1;
    decltype(out1) out2;

    trivial_raster_triangle<>(array{p0, p1, p2}, back_inserter(out1), predicate);
    integral_incremental_raster_triangle<>(
        array{p0, p1, p2},
        back_inserter(out2),
        predicate

    );

    for(const auto [b1, b2] : views::zip(out1, out2))
    {
        REQUIRE(b1.point == b2.point);

        for(auto i = 0; i < 3; ++i) REQUIRE(b1.coordinate[i] == b2.coordinate[i]);
    }

    REQUIRE(out1.size() == out2.size());
}

SCENARIO("raster triangle", "[raster_triangle]")
{
    constexpr auto size = 100;

    const auto [p0, p1, p2] = GENERATE(take(size, Catch::random_triangle(-100, 100)));

    GIVEN(format("triangle p0 = {}, p1 = {}, p2 = {}", p0, p1, p2))
    {
        WHEN("rasterize it") { validate(p0, p1, p2); }

        AND_WHEN("with predicate")
        {
            constexpr ivec2 window_max{size / 2};
            validate(p0, p1, p2, raster_triangle_windowed_predicate<int>{-window_max, window_max});
        }
    }
}