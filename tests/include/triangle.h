#pragma once

#include "test.h"

namespace graphics::test
{
    struct triangle
    {
        glm::ivec2 p0{};
        glm::ivec2 p1{};
        glm::ivec2 p2{};
    };
}

namespace Catch
{
    class random_triangle_generator final : public Generators::IGenerator<graphics::test::triangle>
    {
        random_glm_vec_generator<2, int> m_gen;
        graphics::test::triangle current_{};

    public:
        random_triangle_generator(int, int);

        const type& get() const override;

        bool next() override;
    };

    Generators::GeneratorWrapper<graphics::test::triangle> random_triangle(int, int);
}