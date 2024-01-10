#include "benchmark.h"
#include "triangle.h"
#include "graphics/rasterization/raster_triangle.h"

using namespace std;
using namespace stdsharp;
using namespace glm;
using namespace graphics::rasterization;

namespace
{
    auto generate_random_data()
    {
        std::array<graphics::test::triangle, 1000> a{};
        auto gen = Catch::random_triangle(-5, 5);

        for(auto& t : a)
        {
            t = gen.get();
            gen.next();
        }

        return a;
    };

    const auto& get_random_data()
    {
        static const auto& random_data = generate_random_data();
        return random_data;
    }
}

template<auto Fn>
constexpr auto triangle_raster_test = [](benchmark::State& state)
{
    std::vector<glm::ivec2> out;
    std::vector<glm::vec2> out_b;
    auto& data = get_random_data();

    for(std::size_t i = 0; auto _ : state)
    {
        const auto [p0, p1, p2] = data[i];

        out.clear();
        out_b.clear();

        Fn(p0, p1, p2, out.begin(), out_b.begin());

        i = i == data.size() ? 0 : i + 1;
    }

    benchmark::DoNotOptimize(out);
    benchmark::DoNotOptimize(out_b);
};

BENCHMARK(triangle_raster_test<trivial_raster_triangle<>>);
BENCHMARK(triangle_raster_test<fast_raster_triangle<>>);
BENCHMARK(triangle_raster_test<precise_raster_triangle<>>);