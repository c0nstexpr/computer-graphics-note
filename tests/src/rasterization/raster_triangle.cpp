#include "test.h"
#include "graphics/rasterization/raster_triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

struct triangle
{
    ivec2 p0{};
    ivec2 p1{};
    ivec2 p2{};
};

class random_triangle_generator final : public Catch::Generators::IGenerator<triangle>
{
    Catch::random_glm_vec_generator<2, int> m_gen;
    triangle current_{};

public:
    random_triangle_generator(const int low, const int high): m_gen(low, high) { next(); }

    const type& get() const override { return current_; }

    bool next() override
    {
        current_.p0 = m_gen.get();

        do {
            m_gen.next();
            current_.p1 = m_gen.get();
        } while(current_.p0 == current_.p1);

        const auto line_a = current_.p1.y - current_.p0.y;
        const auto line_b = current_.p0.x - current_.p1.x;
        const auto line_c = line_a * current_.p0.x + line_b * current_.p0.y;

        do {
            m_gen.next();
            current_.p2 = m_gen.get();
        } while((line_a * current_.p2.x + line_b * current_.p2.y) == line_c);

        return true;
    }
};

Catch::Generators::GeneratorWrapper<triangle> random_triangle(const int low, const int high)
{
    return new random_triangle_generator{low, high};
}

SCENARIO("raster triangle", "[raster_triangle]")
{
    // const auto [p0, p1, p2] = GENERATE(take(10, random_triangle(-5, 5)));

    const ivec2 p0{4, 3}, p1{2, 1}, p2{-2, 2};

    GIVEN(format("triangle p0 = {}, p1 = {}, p2 = {}", p0, p1, p2))
    WHEN("rasterize it")
    {
        vector<ivec2> out1, out2, out3;
        vector<vec3> out_bcoor1, out_bcoor2, out_bcoor3;
        constexpr auto epsilon = 0.001f;

        trivial_raster_triangle<>(p0, p1, p2, back_inserter(out1), back_inserter(out_bcoor1));
        fast_raster_triangle<>(p0, p1, p2, back_inserter(out2), back_inserter(out_bcoor2));
        precise_raster_triangle<>(p0, p1, p2, back_inserter(out3), back_inserter(out_bcoor3));

        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out2));
        REQUIRE_THAT(out1, Catch::Matchers::RangeEquals(out3));

        REQUIRE(out_bcoor1.size() == out_bcoor2.size());
        REQUIRE(out_bcoor1.size() == out_bcoor3.size());

        for(const auto [b1, b2, b3] : views::zip(out_bcoor1, out_bcoor2, out_bcoor3))
            for(auto i = 0; i < b1.length(); ++i)
            {
                REQUIRE_THAT(b1[i], Catch::Matchers::WithinAbs(b2[i], epsilon));
                REQUIRE_THAT(b1[i], Catch::Matchers::WithinAbs(b3[i], epsilon));
            }
    }
}